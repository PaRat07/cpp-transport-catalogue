#include "input_reader.h"

using namespace std;

namespace transport_catalogue {
namespace detail {
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
            const int old_size = data.data_bus.first.size();
            data.data_bus.first.resize(old_size * 2 - 1);
            copy(data.data_bus.first.rbegin() + old_size, data.data_bus.first.rend(), data.data_bus.first.begin() + old_size);
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
        lon = stod(string(str.substr(0, str.find_first_of(','))));
        str.remove_prefix(str.find_first_of(',') == str.npos ? str.size() : str.find_first_of(',') + 2);
        vector<pair<string, int>> dists;
        while (str.size() > 1) {
            const int dist = stoi(string(str.substr(0, str.find_first_of('m'))));
            string_view dist_to = str.substr(str.find_first_of('t') + 3, str.find_first_of(',') - str.find_first_of('t') - 3);
            str.remove_prefix(str.find_first_of(',') == str.npos ? str.size() : str.find_first_of(',') + 2);
            dists.emplace_back(string(dist_to), dist);
        }
        data.data_stop = tuple(string(stop_name), lat, lon, dists );
    }
}
}

std::istream& operator>>(std::istream& in, AddData& q) {
    std::string query;
    getline(in, query);
    string_view query_sv = query;
    std::string_view type_query = query_sv.substr(0, query_sv.find_first_of(' '));
    q.type = (type_query == "Stop"s ? AddDataType::ADD_STOP : AddDataType::ADD_WAY);
    detail::SplitData(q, query_sv.substr(type_query.size() + 1));
    return in;
}

void GetData(TransportCatalogue &cat) {
    int query_count;
    string str;
    cin >> query_count;
    getline(cin, str);
    vector<AddData> bus_add;
    vector<pair<pair<string, string>, int>> distances;
    for (int i = 0; i < query_count; ++i) {
        AddData d;
        cin >> d;
        if (d.type == AddDataType::ADD_WAY) {
            bus_add.push_back(d);
        } else {
            double lat, lng;
            string stop_name;
            vector<pair<string, int>> dists;
            tie(stop_name, lat, lng, dists) = d.data_stop;
            cat.AddStop(stop_name, lat, lng);
            distances.resize(distances.size() + dists.size());
            transform(dists.begin(), dists.end(), distances.end() - dists.size(), [&stop_name](const pair<string, int> &p) {
                return pair(pair(stop_name, p.first), p.second);
            });
        }
    }
    for (const auto i : distances) {
        cat.SetDistBetweenStops(i.first.first, i.first.second, i.second);
    }
    for (const auto &i: bus_add) {
        cat.AddBus(i.data_bus.second, i.data_bus.first);
    }
}
}