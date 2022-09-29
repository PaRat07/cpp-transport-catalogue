#include <iostream>
#include <stdexcept>
#include <iomanip>

#include "transport_catalogue.h"
#include "json_reader.h"
#include "request_handler.h"
#include "json.h"

using namespace std;
using namespace transport_catalogue;

int main() {
    transport_catalogue::TransportCatalogue cat;
    auto query = json::Load(cin);
    MapSettings settings(query.GetRoot().AsDict().at("render_settings").AsDict());
    renderer::MapRenderer map(settings);
    RequestHandler handler(cat, map);
    JsonReader reader(cat, map);
    reader.AddData(query.GetRoot().AsDict().at("base_requests").AsArray());
    json::Print(json::Document(handler.SolveQueries(query.GetRoot().AsDict().at("stat_requests").AsArray())), cout);
}