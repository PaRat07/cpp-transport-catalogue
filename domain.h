#pragma once

#include <string>
#include <vector>
#include <cmath>
#include "geo.h"

/*
 * В этом файле вы можете разместить классы/структуры, которые являются частью предметной области (domain)
 * вашего приложения и не зависят от транспортного справочника. Например Автобусные маршруты и Остановки. 
 *
 * Их можно было бы разместить и в transport_catalogue.h, однако вынесение их в отдельный
 * заголовочный файл может оказаться полезным, когда дело дойдёт до визуализации карты маршрутов:
 * визуализатор карты (map_renderer) можно будет сделать независящим от транспортного справочника.
 *
 * Если структура вашего приложения не позволяет так сделать, просто оставьте этот файл пустым.
 *
 */
struct Stop {
    Stop(std::string_view n, double lat, double lng);
    Stop(Stop&& stop);
    Stop(const Stop&) = default;
    std::string name;
    geo::Coordinates coordinates;
    bool operator==(const Stop &other) const {
        return other.name == name && other.coordinates == coordinates;
    }
    Stop& operator=(const Stop &other) = default;
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

struct StopPairHasher {
    StopHasher stop_hash;
    size_t operator() (std::pair<const Stop*, const Stop*> p) const {
        return stop_hash(p.first) + stop_hash(p.second) * 37;
    }
};

struct Bus {
    Bus(std::string_view n, const std::vector<const Stop*> &s);
    Bus(Bus&& bus) noexcept;
    Bus(const Bus& bus) = default;
    Bus() = default;
    std::string name;
    std::vector<const Stop*> stops;
    bool operator==(const Bus &other) const {
        return other.name == name && other.stops == stops;
    }
    Bus& operator=(const Bus &other) = default;
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