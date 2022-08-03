#include <iostream>
#include <stdexcept>
#include <iomanip>

#include "transport_catalogue.h"
#include "input_reader.h"
#include "stat_reader.h"

using namespace std;

int main() {
    cout << setprecision(6);
    TransportCatalogue cat;
    int query_count;
    string str;
    cin >> query_count;
    getline(cin, str);
    vector<AddData> bus_add;
    for (int i = 0; i < query_count; ++i) {
        AddData d;
        cin >> d;
        if (d.type == AddDataType::ADD_WAY) {
            bus_add.push_back(d);
        } else {
            double lat, lng;
            string stop_name;
            tie(stop_name, lat, lng) = d.data_stop;
            cat.AddStop(stop_name, lat, lng);
        }
    }
    for (const auto &i: bus_add) {
        cat.AddBus(i.data_bus.second, i.data_bus.first);
    }

    cin >> query_count;
    getline(cin, str);
    for (int i = 0; i < query_count; ++i) {
        Query q;
        cin >> q;
        if (q.stop_name.empty()) {
            const auto ans = cat.GetDataForBus(q.bus_name);
            string_view bus_name;
            size_t stops_amount, unique_stops_amount;
            double route_length;
            tie(bus_name, stops_amount, unique_stops_amount, route_length) = ans;
            if (stops_amount != 0) {
                cout << "Bus " << bus_name << ": " << stops_amount << " stops on route, " << unique_stops_amount
                     << " unique stops, " << route_length << " route length" << "\n";
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
                    cout << "no buses" << endl;
                }
                else {
                    cout << "buses";
                    for (string_view bus : buses) {
                        cout << " " << bus;
                    }
                    cout << endl;
                }
            }
            else {
                cout << "not found" << endl;
            }
        }
    }
}