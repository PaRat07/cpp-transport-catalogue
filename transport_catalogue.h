#pragma once

#include <iostream>
#include <vector>
#include <deque>
#include <cmath>
#include <algorithm>
#include <unordered_map>
#include <unordered_set>
#include <set>
#include <optional>

#include "geo.h"
#include "graph.h"
#include "domain.h"
#include "json_builder.h"

namespace transport_catalogue {

class TransportCatalogue {
public:
    // Добавляет автобус в базу данных
    const Bus* AddBus(std::string_view bus_name, const std::vector<std::string> &stops, bool is_roundtrip);

    // Добавляет остановку в базу данных
    void AddStop(std::string_view stop_name, double lat, double lng);

    // Возвращает данные о маршруте по его имени
    std::optional<BusStat> GetDataForBus(std::string_view bus) const;

    // Возвращает данные об остановке по её имени
    std::optional<StopsStat> GetDataForStop(std::string_view stop_name) const;

    // Устанавливает дистанцию от одной остановки к другой
    void SetDistBetweenStops(std::string_view lhs, std::string_view rhs, int dist);

    // Возвращает дистанцию от одной остановки к другой(в случае если запрошена дистанция от А до Б а указанно расстояние от А до Б и от Б до А(они могут быть разными) возвращается расстояние от А до Б)
    int GetDistBetweenStops(const Stop* lhs, const Stop* rhs) const;

    json::Document GetWayFromTo(std::string_view from, std::string_view to);

private:
    const Bus* SearchBusByName(std::string_view name) const;
    const Stop* SearchStopByName(std::string_view name) const;
    std::deque<Bus> buses_;
    std::deque<Stop> stops_;
    std::unordered_map<std::string_view, const Stop*> stops_to_name_;
    std::unordered_map<std::string_view, const Bus*> bus_to_name;
    std::unordered_map<std::string_view, std::set<std::string_view, std::less<>>> unique_buses_for_stop_;
    std::unordered_map<const Bus*, std::unordered_set<const Stop*>, BusHasher> unique_stops_for_bus_;
    std::unordered_map<std::pair<const Stop*, const Stop*>, int, StopPairHasher> dist_between_stops_;
    std::unordered_map<const Stop*, graph::Edge<const Stop*>> edge_to_bus_;
    graph::DirectedWeightedGraph<const Stop*> graph_;
    std::pair<json::Document, int> GetWay(const Stop *from, const Stop *to);
};
}