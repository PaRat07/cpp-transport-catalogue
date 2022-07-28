#pragma once

#include <iostream>
#include <vector>
#include <deque>
#include <cmath>
#include <unordered_map>
#include <unordered_set>

struct Stop {
    Stop(std::string_view n, double lat, double lon);
    Stop(Stop&& stop);
    std::string name;
    double latitude;
    double longitude;
};

struct StopHasher {
    std::hash<std::string> str_hasher;
    std::hash<double> double_hasher;
    size_t operator() (const Stop* stop) {
        return str_hasher(stop->name) + double_hasher(stop->latitude) * 37 + double_hasher(stop->longitude) * 37 * 37;
    }
};

struct Bus {
    Bus(std::string_view n, std::vector<const Stop*> s);
    Bus(Bus&& bus);
    std::string name;
    std::vector<const Stop*> stops;
};

struct BusHasher {
    std::hash<std::string> str_hasher;
    StopHasher stop_hasher;
    size_t operator() (const Bus* bus) {
        size_t ans = str_hasher(bus->name);
        for (int i = 0; i < bus->stops.size(); ++i) {
            ans += stop_hasher(bus->stops[i]) * std::pow(37, i + 1);
        }
        return ans;
    }
};

class TransportCatalogue {
public:
    void AddBus(std::string_view bus_name, std::vector<std::string_view> stops);
    void AddStop(std::string_view stop_name, double latitude, double longtitude);
    std::tuple<std::string_view, int, int, double> GetDataForBus(std::string_view bus);
private:
    std::deque<Stop> stops_;
    std::deque<Bus> buses_;
    std::unordered_map<const Stop*, std::unordered_set<const Bus*, BusHasher>, StopHasher> buses_for_stop_;
//    std::unordered_map<const Bus*, std::vector<const Stop*>, BusHasher> stops_for_bus_;
    std::unordered_map<const Bus*, std::unordered_set<const Stop*>, BusHasher> unique_stops_for_bus_;
};