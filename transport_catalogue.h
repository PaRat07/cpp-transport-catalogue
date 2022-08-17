#pragma once

#include <iostream>
#include <vector>
#include <deque>
#include <cmath>
#include <algorithm>
#include <unordered_map>
#include <unordered_set>
#include <set>

#include "geo.h"
#include "domain.h"

namespace transport_catalogue {

class TransportCatalogue {
public:
    void AddBus(std::string_view bus_name, const std::vector<std::string> &stops);
    void AddStop(std::string_view stop_name, double lat, double lng);
    std::tuple<std::string_view, size_t, size_t, double, double> GetDataForBus(std::string_view bus) const;
    std::tuple<bool, std::string_view, std::set<std::string_view, std::less<>>> GetDataForStop(std::string_view stop_name) const;
    void SetDistBetweenStops(std::string_view lhs, std::string_view rhs, int dist);
    int GetDistBetweenStops(const Stop* lhs, const Stop* rhs) const;
private:
    const Bus* SearchBusByName(std::string_view name) const;
    const Stop* SearchStopByName(std::string_view name) const;
    std::deque<Stop> stops_;
    std::deque<Bus> buses_;
    std::unordered_map<std::string_view, const Stop*> stops_to_name_;
    std::unordered_map<std::string_view, const Bus*> bus_to_name;
    std::unordered_map<std::string_view, std::set<std::string_view, std::less<>>> unique_buses_for_stop_;
    std::unordered_map<const Bus*, std::unordered_set<const Stop*>, BusHasher> unique_stops_for_bus_;
    std::unordered_map<std::pair<const Stop*, const Stop*>, int, StopPairHasher> dist_between_stops_;
};
}