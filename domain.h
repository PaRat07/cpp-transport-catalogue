#pragma once

#include <string>
#include <vector>
#include <cmath>
#include <utility>
#include <set>
#include "geo.h"
#include "svg.h"
#include "json.h"

// Хранит данные об остановке
struct Stop {
    Stop(std::string_view n, double lat, double lng);
    Stop(Stop&& stop);
    Stop(const Stop&) = default;
    std::string name;
    geo::Coordinates coordinates;
    bool operator==(const Stop &other) const;
    Stop& operator=(const Stop &other) = default;
};

// Хеширует остановки
struct StopHasher {
    std::hash<std::string> str_hasher;
    std::hash<double> double_hasher;
    size_t operator() (const Stop& stop) const;
    size_t operator() (const Stop* stop) const;
};

// Хэширует пару остановок
struct StopPairHasher {
    StopHasher stop_hash;
    size_t operator() (std::pair<const Stop*, const Stop*> p) const;
};

// Хранит даные о маршруте
struct Bus {
    Bus(std::string_view n, const std::vector<const Stop*> &s, bool i_r);
    Bus(Bus&& bus) noexcept;
    Bus(const Bus& bus) = default;
    Bus() = default;
    std::string name;
    std::vector<const Stop*> stops;
    bool is_roundtrip;
    bool operator==(const Bus &other) const;
    Bus& operator=(const Bus &other) = default;
};

// Хэширует маршрут
struct BusHasher {
    std::hash<std::string> str_hasher;
    StopHasher stop_hasher;
    size_t operator() (const Bus &bus) const;
    size_t operator() (const Bus* const bus) const;
};

// Хранит данные, которые возвращаются при запросе маршрута по имени
struct BusStat {
    BusStat(std::string_view n, size_t s_a, size_t u_s_a, size_t r_l, double c);

    std::string_view name;
    size_t stops_amount;
    size_t unique_stops_amount;
    size_t route_length;
    double curvature;
};

// Хранит данные, которые возвращаются при запросе остановки по имени
struct StopsStat {
    StopsStat(std::string_view n, std::set<std::string_view, std::less<>> b);

    std::string_view name;
    std::set<std::string_view, std::less<>> buses;
};

// Хранит настройки карты
struct MapSettings {
    MapSettings(const json::Dict &data);
    double width;
    double height;
    double padding;
    double line_width;
    double stop_radius;
    int bus_label_font_size;
    svg::Point bus_label_offset;
    int stop_label_font_size;
    svg::Point stop_label_offset;
    svg::Color underlayer_color;
    double underlayer_width;
    std::vector<svg::Color> color_palette;
};