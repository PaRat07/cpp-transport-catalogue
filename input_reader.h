#pragma once

#include <string>
#include <iostream>
#include <vector>
#include <tuple>

using std::literals::string_literals::operator""s;

namespace transport_catalogue {
enum class AddDataType {
    ADD_WAY,
    ADD_STOP
};

struct AddData {
    AddDataType type;
    std::pair<std::vector<std::string>, std::string> data_bus;
    std::tuple<std::string, double, double, std::vector<std::pair<std::string, int>>> data_stop;
};
namespace detail {
void SplitData(AddData& data, std::string_view query);
}

std::istream& operator>>(std::istream& in, AddData& q);
}