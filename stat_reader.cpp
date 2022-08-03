#include "stat_reader.h"

using namespace std;

namespace transport_catalogue {
std::istream& operator>>(std::istream& in, Query& q) {
    std::string query;
    getline(in, query);
    if (query.substr(0, query.find_first_of(' ')) == "Stop"s) {
        q.stop_name = query.substr(5);
    }
    else {
        q.bus_name = query.substr(4);
    }
    return in;
}

void GetAndDoQueries(TransportCatalogue &cat) {
    string str;
    int query_count;
    cin >> query_count;
    getline(cin, str);

    for (int i = 0; i < query_count; ++i) {
        Query q;
        cin >> q;
        if (q.stop_name.empty()) {
            const auto ans = cat.GetDataForBus(q.bus_name);
            string_view bus_name;
            size_t stops_amount, unique_stops_amount;
            double route_length, curvature;
            tie(bus_name, stops_amount, unique_stops_amount, route_length, curvature) = ans;
            if (stops_amount != 0) {
                cout << "Bus " << bus_name << ": " << stops_amount << " stops on route, " << unique_stops_amount
                     << " unique stops, " << route_length << " route length, "  << curvature << " curvature" << "\n";
            } else {
                cout << "Bus " << bus_name << ": not found" << "\n";
            }
        }
        else {
            bool exist;
            string_view stop_name;
            std::set<string_view, less<>> buses;
            tie(exist, stop_name, buses) = cat.GetDataForStop(q.stop_name);
            cout << "Stop " << stop_name << ": ";
            if (exist) {
                if (buses.empty()) {
                    cout << "no buses" << "\n";
                }
                else {
                    cout << "buses";
                    for (string_view bus : buses) {
                        cout << " " << bus;
                    }
                    cout << "\n";
                }
            }
            else {
                cout << "not found" << "\n";
            }
        }
    }
}
}