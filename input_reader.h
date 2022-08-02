#pragma once

#include <string>
#include <iostream>
#include <vector>
#include <tuple>

using std::literals::string_literals::operator""s;

enum class AddDataType {
    ADD_WAY,
    ADD_STOP
};

struct AddData {
    AddDataType type;
    std::pair<std::vector<std::string>, std::string> data_bus;
    std::tuple<std::string, double, double> data_stop;
};

void SplitData(AddData& data, std::string_view query);

std::istream& operator>>(std::istream& in, AddData& q);