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
}