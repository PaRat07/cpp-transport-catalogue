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

class JsonReader {
public:
    JsonReader(transport_catalogue::TransportCatalogue &c,
               renderer::MapRenderer &m,
               const RequestHandler &h);

    // Загружает данные из потока в каталог и карту
    void AddData(const json::Array &data);

    // Возвращает ответы на запросы из потока в виде JSON
    json::Node SolveQueries(const json::Array &data) const;
private:
    transport_catalogue::TransportCatalogue &cat_;
    renderer::MapRenderer &map_;
    const RequestHandler &hand_;
};