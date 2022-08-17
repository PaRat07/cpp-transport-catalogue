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

void TransportCatalogue::AddBus(std::string_view bus_name, const std::vector<std::string> &stops) {
    Bus bus;
    bus.name = bus_name;
    for (const string &name: stops) {
        const auto stop = SearchStopByName(name);
        bus.stops.push_back(stop);
    }
    const Bus *bus_ptr = &buses_.emplace_back(std::move(bus));
    bus_to_name[bus_ptr->name] = bus_ptr;
    for (const Stop *const stop: bus_ptr->stops) {
        unique_buses_for_stop_[stop->name].insert(bus_ptr->name);
        unique_stops_for_bus_[bus_ptr].insert(stop);
    }
}

void TransportCatalogue::AddStop(std::string_view stop_name, double lat, double lng) {
    const Stop *stop = &stops_.emplace_back(stop_name, lat, lng);
    stops_to_name_[stop->name] = stop;
    unique_buses_for_stop_[stop->name];
}

std::tuple<std::string_view, size_t, size_t, double, double>
TransportCatalogue::GetDataForBus(std::string_view bus) const {
    const Bus *bus_ptr = SearchBusByName(bus);
    if (bus_ptr == nullptr) {
        return tuple(bus, 0, 0, 0, 1);
    }
    const double route_length = transform_reduce(bus_ptr->stops.begin(), bus_ptr->stops.end() - 1,
                                                 bus_ptr->stops.begin() + 1,
                                                 0.0, std::plus<>(), [](const Stop *lhs, const Stop *rhs) {
                return ComputeDistance(lhs->coordinates, rhs->coordinates);
            });
    const int real_route_length = transform_reduce(bus_ptr->stops.begin(), bus_ptr->stops.end() - 1,
                                                   bus_ptr->stops.begin() + 1,
                                                   0.0, std::plus<>(), [&](const Stop *lhs, const Stop *rhs) {
                return GetDistBetweenStops(lhs, rhs);
            });
    return {bus, bus_ptr->stops.size(), unique_stops_for_bus_.at(bus_ptr).size(), real_route_length,
            real_route_length / route_length};
}

std::tuple<bool, std::string_view, std::set<std::string_view, std::less<>>>
TransportCatalogue::GetDataForStop(std::string_view stop_name) const {
    if (SearchStopByName(stop_name) == nullptr) {
        return {false, stop_name, {}};
    } else {
        return {true, stop_name, unique_buses_for_stop_.at(stop_name)};
    }
}
}