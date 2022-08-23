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
               renderer::MapRenderer &m);

    // Загружает данные из потока в каталог и карту
    void AddData(const json::Array &data);
private:
    transport_catalogue::TransportCatalogue &cat_;
    renderer::MapRenderer &map_;
};