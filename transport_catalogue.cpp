#include <numeric>
#include "transport_catalogue.h"

using namespace std;

Stop::Stop(Stop &&stop)
    : name(exchange(stop.name, ""))
    , coordinates()
{
}

Stop::Stop(string_view n, double lat, double lng)
    : name(n)
    , coordinates({lat, lng})
{
}

Bus::Bus(Bus &&bus)
    : name(exchange(bus.name, ""s))
    , stops(exchange(bus.stops, {}))
{
}

Bus::Bus(string_view n, std::vector<const Stop *> s)
    : name(n)
    , stops(move(s))
{
}

const Bus* TransportCatalogue::SearchBusByName(std::string_view name) const {
    return bus_to_name.count(name) ? bus_to_name.at(name) : nullptr;
}

const Stop* TransportCatalogue::SearchStopByName(std::string_view name) const {
    return stops_to_name_.at(name);
}

void TransportCatalogue::AddBus(std::string_view bus_name, const std::vector<std::string> &stops) {
    Bus bus;
    bus.name = bus_name;
    for (const string &name : stops) {
        const auto stop = SearchStopByName(name);
        bus.stops.push_back(&*stop);
    }
    const Bus* bus_ptr = &buses_.emplace_back(std::move(bus));
    bus_to_name[bus_ptr->name] = bus_ptr;
    for (const Stop* const stop : bus_ptr->stops) {
        unique_buses_for_stop_[*stop].insert(bus_ptr);
        unique_stops_for_bus_[*bus_ptr].insert(stop);
    }
}

void TransportCatalogue::AddStop(std::string_view stop_name, double lat, double lng) {
    const Stop* stop = &stops_.emplace_back(stop_name, lat, lng);
    stops_to_name_[stop->name] = stop;
}

std::tuple<std::string, size_t, size_t, double> TransportCatalogue::GetDataForBus(std::string_view bus) const {
    const Bus* bus_ptr = SearchBusByName(bus);
    if (bus_ptr == nullptr) {
        return tuple(string(bus), 0, 0, 0);
    }
    const size_t stops_amount = bus_ptr->stops.size();
    const size_t unique_stops_amount = unique_stops_for_bus_.at(*bus_ptr).size();
    const double route_length = transform_reduce(bus_ptr->stops.begin(), bus_ptr->stops.end() - 1, bus_ptr->stops.begin() + 1,
                                                 0.0, std::plus<>(), [](const Stop* lhs, const Stop* rhs) {
                                                                                                return ComputeDistance(lhs->coordinates, rhs->coordinates);
                                                                                            });
    return {string(bus), stops_amount, unique_stops_amount, route_length };
}