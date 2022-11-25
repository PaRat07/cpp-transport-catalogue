#include <fstream>
#include <iostream>
#include <string_view>

#include "transport_catalogue.h"
#include "serialization.h"
#include "json.h"
#include "json_reader.h"
#include "request_handler.h"
#include "json_reader.h"

using namespace std::literals;

void PrintUsage(std::ostream& stream = std::cerr) {
    stream << "Usage: transport_catalogue [make_base|process_requests]\n"sv;
}

int main() {
    string mode;
    cin >> mode;

    if (mode == "make_base"sv) {
        auto query = json::Load(cin);
        RoutingSettings routing_settings(query.GetRoot().AsDict().at("routing_settings").AsDict());
        transport_catalogue::TransportCatalogue cat(routing_settings);
        MapSettings map_settings(query.GetRoot().AsDict().at("render_settings").AsDict());
        renderer::MapRenderer map;
        map.SetMapSettings(map_settings);
        JsonReader reader(cat, map);
        reader.AddData(query.GetRoot().AsDict().at("base_requests").AsArray(), map_settings, {query.GetRoot().AsDict().at("serialization_settings").AsDict().at("file").AsString()});
        cat.GetRoute("Морской вокзал"sv, "Параллельная улица"sv);
    } else if (mode == "process_requests"sv) {
        json::Document query = json::Load(cin);
        transport_catalogue::TransportCatalogue cat(RoutingSettings{});
        renderer::MapRenderer map;
        Deserialize(query.GetRoot().AsDict().at("serialization_settings").AsDict().at("file").AsString(), cat, map);
        cat.GetRoute("Морской вокзал"sv, "Параллельная улица"sv);
        RequestHandler handler{cat, map};
        json::Print(json::Document(handler.SolveQueries(query.GetRoot().AsDict().at("stat_requests").AsArray())), cout);
    } else {
        PrintUsage();
        return 1;
    }
}