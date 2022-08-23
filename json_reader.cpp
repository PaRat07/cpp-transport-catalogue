#include "json_reader.h"

using namespace std;


BusDataForAdd::BusDataForAdd(std::string n, std::vector<std::string> s, bool i_r)
        : name(std::move(n))
        , stops(std::move(s))
        , is_roundtrip(i_r)
{

}

void JsonReader::AddData(const json::Array &data) {
    std::vector<BusDataForAdd> buses_queries;
    vector<pair<pair<string, string>, int>> stop_dists;
    for (const auto &i : data) {
        if (i.AsMap().at("type").AsString() == "Stop") {
            cat_.AddStop(i.AsMap().at("name").AsString(), i.AsMap().at("latitude").AsDouble(), i.AsMap().at("longitude").AsDouble());
            for (const auto &[name, dist] : i.AsMap().at("road_distances").AsMap()) {
                stop_dists.push_back({{i.AsMap().at("name").AsString(), name}, dist.AsInt()});
            }
        }
        else {
            vector<string> stops(i.AsMap().at("stops").AsArray().size());
            for (size_t s = 0; s < stops.size(); ++s) {
                stops[s] = i.AsMap().at("stops").AsArray()[s].AsString();
            }
            buses_queries.emplace_back(i.AsMap().at("name").AsString(), stops, i.AsMap().at("is_roundtrip").AsBool());
        }
    }
    for (const auto &[buses, dist] : stop_dists) {
        cat_.SetDistBetweenStops(buses.first, buses.second, dist);
    }
    for (const auto &i : buses_queries) {
        map_.AddBus(*cat_.AddBus(i.name, i.stops, i.is_roundtrip));
    }
}

JsonReader::JsonReader(transport_catalogue::TransportCatalogue &c, renderer::MapRenderer &m)
        : cat_(c)
        , map_(m)
{
}
