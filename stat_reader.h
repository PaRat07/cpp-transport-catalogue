#pragma once
#include <string>
#include <iostream>
#include <vector>
#include <cassert>

#include "transport_catalogue.h"

namespace transport_catalogue {
struct Query {
    std::string bus_name;
    std::string stop_name;
};

void GetAndDoQueries(TransportCatalogue &cat);

std::istream& operator>>(std::istream& in, Query& q);
}