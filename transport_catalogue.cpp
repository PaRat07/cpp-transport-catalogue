#include <numeric>
#include "transport_catalogue.h"

using namespace std;

namespace transport_catalogue {
void TransportCatalogue::SetDistBetweenStops(string_view lhs, string_view rhs, int dist) {
    dist_between_stops_[{SearchStopByName(lhs), SearchStopByName(rhs)}] = dist;
}

int TransportCatalogue::GetDistBetweenStops(const Stop *lhs, const Stop *rhs) const {
    return dist_between_stops_.count({lhs, rhs}) ? dist_between_stops_.at({lhs, rhs}) : dist_between_stops_.at({rhs, lhs});
}

const Bus *TransportCatalogue::SearchBusByName(std::string_view name) const {
    return bus_to_name.count(name) ? bus_to_name.at(name) : nullptr;
}

const Stop *TransportCatalogue::SearchStopByName(std::string_view name) const {
    return stops_to_name_.count(name) ? stops_to_name_.at(name) : nullptr;
}

const Bus* TransportCatalogue::AddBus(std::string_view bus_name, const std::vector<std::string> &stops, bool is_roundtrip) {
    Bus bus;
    bus.name = bus_name;
    bus.is_roundtrip = is_roundtrip;
    for (const string &name: stops) {
        const auto stop = SearchStopByName(name);
        bus.stops.push_back(stop);
    }
    const Bus *bus_ptr = &buses_.emplace_back(bus);
    bus_to_name[bus_ptr->name] = bus_ptr;
    for (const Stop *const stop: bus_ptr->stops) {
        unique_buses_for_stop_[stop->name].insert(bus_ptr->name);
        unique_stops_for_bus_[bus_ptr].insert(stop);
    }
    return bus_ptr;
}

void TransportCatalogue::AddStop(std::string_view stop_name, double lat, double lng) {
    const Stop *stop = &stops_.emplace_back(stop_name, lat, lng);
    stops_to_name_[stop->name] = stop;
    unique_buses_for_stop_[stop->name];
}

optional<BusStat> TransportCatalogue::GetDataForBus(std::string_view bus) const {
    const Bus *bus_ptr = SearchBusByName(bus);
    if (bus_ptr == nullptr) {
        return nullopt;
    }
    double route_length = transform_reduce(bus_ptr->stops.begin(), bus_ptr->stops.end() - 1,
                                                 bus_ptr->stops.begin() + 1,
                                                 0.0, std::plus<>(), [](const Stop *lhs, const Stop *rhs) {
                return ComputeDistance(lhs->coordinates, rhs->coordinates);
            });
    double real_route_length = transform_reduce(bus_ptr->stops.begin(), bus_ptr->stops.end() - 1,
                                                   bus_ptr->stops.begin() + 1,
                                                   0.0, std::plus<>(), [&](const Stop *lhs, const Stop *rhs) {
                return GetDistBetweenStops(lhs, rhs);
            });    if (!bus_ptr->is_roundtrip) {
        real_route_length += transform_reduce(bus_ptr->stops.rbegin(), bus_ptr->stops.rend() - 1,
                                              bus_ptr->stops.rbegin() + 1,
                                              0.0, std::plus<>(), [&](const Stop *lhs, const Stop *rhs) {
                    return GetDistBetweenStops(lhs, rhs);
                });;
        route_length *= 2;
    }
    return BusStat(bus, (bus_ptr->is_roundtrip ? bus_ptr->stops.size() : bus_ptr->stops.size() * 2 - 1), unique_stops_for_bus_.at(bus_ptr).size(), real_route_length,
            real_route_length / route_length);
}

std::optional<StopsStat>
TransportCatalogue::GetDataForStop(std::string_view stop_name) const {
    if (SearchStopByName(stop_name) == nullptr) {
        return nullopt;
    } else {
        return StopsStat(stop_name, unique_buses_for_stop_.at(stop_name));
    }
}

}