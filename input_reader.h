#pragma once

#include <string>
#include <iostream>

using std::literals::string_literals::operator""s;

enum class AddDataType {
    ADD_WAY,
    ADD_STOP
};

struct AddData {
    AddDataType type;
    std::string data;
};

enum class QueryType {
    SEARCH_WAY_BY_NAME,
    SEARCH_STOP_BY_NAME,
    GET_WAY_INFO
};

struct Query {
    QueryType type;
    std::string data;
};

std::istream& operator>>(std::istream& in, AddData& q);

std::istream& operator>>(std::istream& in, Query& q);