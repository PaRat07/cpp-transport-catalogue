#include "stat_reader.h"

using namespace std;

std::istream& operator>>(std::istream& in, Query& q) {
    getline(in, q.bus_name);
    q.bus_name = string(q.bus_name.substr(q.bus_name.find_first_of(' ') + 1));
    return in;
}