#pragma once

#include <iostream>

#include "json.h"
#include "transport_catalogue.h"
#include "map_renderer.h"
#include "request_handler.h"

struct BusDataForAdd {
    BusDataForAdd(std::string n, std::vector<std::string> s, bool i_r);
    std::string name;
    std::vector<std::string> stops;
    bool is_roundtrip;
};

// Загружает данные из потока в каталог и карту
void AddData(transport_catalogue::TransportCatalogue &cat, const json::Array &data, renderer::MapRenderer &map);

// Возвращает ответы на запросы из потока в виде JSON
json::Node SolveQueries(const RequestHandler &hand, const json::Array &data);