#include <iostream>
#include <stdexcept>
#include <iomanip>

#include "transport_catalogue.h"
#include "input_reader.h"
#include "stat_reader.h"

using namespace std;
using namespace transport_catalogue;

int main() {
    cout << setprecision(6);
    TransportCatalogue cat;
    GetData(cat);
    GetAndDoQueries(cat);
}