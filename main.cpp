#include <iostream>
#include <stdexcept>

#include "transport_catalogue.h"
#include "input_reader.h"

using namespace std;

int main() {
    TransportCatalogue cat;
    int query_count;
    cin >> query_count;
    for (int i = 0; i < query_count; ++i) {
        AddData d;
        cin >> d;
        if (d.type == AddDataType::ADD_WAY) {
            cat.AddBus(d.data);
        }
        else {
            cat.AddStop(d.data);
        }
    }
    cin >> query_count;
    for (int i = 0; i < query_count; ++i) {
        Query q;
        cin >> q;
        if (q.type == QueryType::SEARCH_STOP_BY_NAME) {

        }
    }
}