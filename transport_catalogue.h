#pragma once

#include <iostream>
#include <vector>
#include <deque>
#include <cmath>
#include <algorithm>
#include <unordered_map>
#include <unordered_set>

#include "geo.h"

struct Stop {
    Stop(std::string_view n, double lat, double lng);
    Stop(Stop&& stop);
    Stop(const Stop&) = default;
    std::string name;
    Coordinates coordinates;
    bool operator==(const Stop &other) const {
        return other.name == name && other.coordinates == coordinates;
    }
    Stop& operator=(const Stop &other) {
        name = other.name;
        coordinates = other.coordinates;
        return *this;
    }
};

struct StopHasher {
    std::hash<std::string> str_hasher;
    std::hash<double> double_hasher;
    size_t operator() (const Stop& stop) const {
        return str_hasher(stop.name) + double_hasher(stop.coordinates.lat) * 37 + double_hasher(stop.coordinates.lng) * 37 * 37;
    }
    size_t operator() (const Stop* stop) const {
        return str_hasher(stop->name) + double_hasher(stop->coordinates.lat) * 37 + double_hasher(stop->coordinates.lng) * 37 * 37;
    }
};

struct Bus {
    Bus(std::string_view n, std::vector<const Stop*> s);
    Bus(Bus&& bus);
    Bus(const Bus& bus) = default;
    Bus() = default;
    std::string name;
    std::vector<const Stop*> stops;
    bool operator==(const Bus &other) const {
        return other.name == name && other.stops == stops;
    }
    Bus& operator=(const Bus &other) {
        name = other.name;
        stops = other.stops;
        return *this;
    }
};

struct BusHasher {
    std::hash<std::string> str_hasher;
    StopHasher stop_hasher;
    size_t operator() (const Bus &bus) const {
        size_t ans = str_hasher(bus.name);
        for (int i = 0; i < bus.stops.size(); ++i) {
            ans += stop_hasher(*bus.stops[i]) * std::pow(37, i + 1);
        }
        return ans;
    }
    size_t operator() (const Bus* const bus) const {
        size_t ans = str_hasher(bus->name);
        for (int i = 0; i < bus->stops.size(); ++i) {
            ans += stop_hasher(*bus->stops[i]) * std::pow(37, i + 1);
        }
        return ans;
    }
};

class TransportCatalogue {
public:
    void AddBus(std::string_view bus_name, const std::vector<std::string> &stops);
    void AddStop(std::string_view stop_name, double lat, double lng);
    std::tuple<std::string, size_t, size_t, double> GetDataForBus(std::string_view bus) const;
private:
    const Bus* SearchBusByName(std::string_view name) const;
    const Stop* SearchStopByName(std::string_view name) const;
    std::deque<Stop> stops_;
    std::deque<Bus> buses_;
    std::unordered_map<std::string_view, const Stop*, std::hash<std::string_view>> stops_to_name_;
    std::unordered_map<std::string_view , const Bus*, std::hash<std::string_view>> bus_to_name;
    std::unordered_map<Stop, std::unordered_set<const Bus*, BusHasher>, StopHasher> unique_buses_for_stop_;
    std::unordered_map<Bus, std::unordered_set<const Stop*>, BusHasher> unique_stops_for_bus_;
};