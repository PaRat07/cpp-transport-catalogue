#pragma once

#include <iostream>

#include "json.h"
#include "transport_catalogue.h"
/*
 * Здесь можно разместить код наполнения транспортного справочника данными из JSON,
 * а также код обработки запросов к базе и формирование массива ответов в формате JSON
 */

struct BusDataForAdd {
    BusDataForAdd(std::string n, std::vector<std::string> s)
        : name(std::move(n))
        , stops(std::move(s))
    {

    }
    std::string name;
    std::vector<std::string> stops;
};


struct Query {
    Query(std::istream &in);
    json::Document data;
};


void SolveQuery(transport_catalogue::TransportCatalogue &cat, Query q, std::ostream &out);