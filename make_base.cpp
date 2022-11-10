#include <iostream>
#include <stdexcept>
#include <iomanip>

#include "transport_catalogue.h"
#include "serialization.h"
#include "json.h"

using namespace std;
using namespace transport_catalogue;

int main() {
    auto query = json::Load(cin);
    transport_catalogue::TransportCatalogue cat(RoutingSettings(query.GetRoot().AsDict().at("routing_settings").AsDict()));
    MapSettings settings(query.GetRoot().AsDict().at("render_settings").AsDict());
}