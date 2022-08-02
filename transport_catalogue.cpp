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

const auto TransportCatalogue::SearchBusByName(std::string_view name) const {
    return find_if(buses_.begin(), buses_.end(), [name] (const Bus &bus) {
        return bus.name == name;
    });
}

const auto TransportCatalogue::SearchStopByName(std::string_view name) const {
    return find_if(stops_.begin(), stops_.end(), [name] (const Stop & stop) {
        return stop.name == name;
    });
}

void TransportCatalogue::AddBus(std::string_view bus_name, const std::vector<std::string> &stops) {
    Bus bus;
    bus.name = bus_name;
    for (string_view name : stops) {
        const auto stop = SearchStopByName(name);
        bus.stops.push_back(&*stop);
    }
    const Bus* bus_ptr = &buses_.emplace_back(std::move(bus));
    for (const Stop* const stop : bus_ptr->stops) {
        unique_buses_for_stop_[*stop].insert(bus_ptr);
        unique_stops_for_bus_[*bus_ptr].insert(stop);
    }
}

void TransportCatalogue::AddStop(std::string_view stop_name, double lat, double lng) {
    stops_.emplace_back(stop_name, lat, lng);
}

std::tuple<bool, std::string, size_t, size_t, double> TransportCatalogue::GetDataForBus(std::string_view bus) const {
    if (SearchBusByName(bus) == buses_.end()) {
        return tuple(false, string(bus), 0, 0, 0);
    }
    const size_t stops_amount = SearchBusByName(bus)->stops.size();
    const size_t unique_stops_amount = unique_stops_for_bus_.at(*SearchBusByName(bus)).size();
    const double route_length = transform_reduce(SearchBusByName(bus)->stops.begin(), SearchBusByName(bus)->stops.end() - 1, SearchBusByName(bus)->stops.begin() + 1,
                                                 0.0, std::plus<>(), [](const Stop* lhs, const Stop* rhs) {
                                                                                                return ComputeDistance(lhs->coordinates, rhs->coordinates);
                                                                                            });
    return { true, string(bus), stops_amount, unique_stops_amount, route_length };
}