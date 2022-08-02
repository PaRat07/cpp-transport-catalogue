#include "input_reader.h"

using namespace std;

void SplitData(AddData& data, std::string_view str) {
    if (data.type == AddDataType::ADD_WAY){
        data.data_bus.second = str.substr(0, str.find_first_of(':'));
        str.remove_prefix(data.data_bus.second.size() + 2);

        int64_t pos_end = str.npos;
        if (str.find('-') != pos_end) {
            while (!str.empty()) {
                data.data_bus.first.emplace_back(str.substr(0, str.find_first_of('-') - 1));
                str.remove_prefix(str.find_first_of('-') == pos_end ? str.size() : str.find_first_of('-') + 2);
            }
            for (int i = data.data_bus.first.size() - 2; i >= 0; --i) {
                data.data_bus.first.push_back(data.data_bus.first[i]);
            }
        }
        else {
            while (!str.empty()) {
                data.data_bus.first.emplace_back(str.substr(0, str.find_first_of('>') - 1));
                str.remove_prefix(str.find_first_of('>') == pos_end ? str.size() : str.find_first_of('>') + 2);
            }
        }
    }
    else {
        string_view stop_name = str.substr(0, str.find_first_of(':'));
        str.remove_prefix(str.find_first_of(':') + 2);
        double lat, lon;
        lat = stod(string(str.substr(0, str.find_first_of(','))));
        str.remove_prefix(str.find_first_of(',') + 2);
        lon = stod(string(str));
        data.data_stop = { string(stop_name), lat, lon };
    }
}

std::istream& operator>>(std::istream& in, AddData& q) {
    std::string query;
    getline(in, query);
    std::string_view type_query = query;
    type_query = type_query.substr(0, type_query.find_first_of(' '));
    if (type_query == "Stop"s) {
        q.type = AddDataType::ADD_STOP;
    }
    else if (type_query == "Bus"s) {
        q.type = AddDataType::ADD_WAY;
    }
    else {
        throw std::invalid_argument("incorrect query"s);
    }

    SplitData(q, query.substr(type_query.size() + 1));
    return in;
}