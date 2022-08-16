#pragma once

#include <iostream>

#include "json.h"
#include "transport_catalogue.h"
/*
 * Здесь можно разместить код наполнения транспортного справочника данными из JSON,
 * а также код обработки запросов к базе и формирование массива ответов в формате JSON
 */

struct Query {
    explicit Query(const json::Document &doc)
        : data(doc)
    {

    }
    Query& operator=(const json::Document &doc);
    json::Document data;
};

std::istream &operator>>(std::istream &out, Query &q);

void SolveQuery(transport_catalogue::TransportCatalogue &cat, Query q);