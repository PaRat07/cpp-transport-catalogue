#include "input_reader.h"

std::istream& operator>>(std::istream& in, AddData& q) {
    std::string type_query;
    in >> type_query;
    if (type_query == "Stop"s) {
        q.type = AddDataType::ADD_STOP;
    }
    else if (type_query == "Bus"s) {
        q.type = AddDataType::ADD_WAY;
    }
    else {
        throw std::invalid_argument("incorrect query"s);
    }
    getline(in, q.data);
}

std::istream& operator>>(std::istream& in, Query& q) {
    std::string type_query;
    in >> type_query;
    if (type_query == "Bus"s) {
        q.type = QueryType::SEARCH_WAY_BY_NAME;
    }
    else if (type_query == "Stop"s) {
        q.type = QueryType::SEARCH_STOP_BY_NAME;
    }
    else {
        q.type = QueryType::GET_WAY_INFO;
    }
    getline(in, q.data);
}