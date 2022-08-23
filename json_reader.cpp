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

json::Node JsonReader::SolveQueries(const json::Array &data) const {
    json::Array ans;
    for (const json::Node &i : data) {
        if (i.AsMap().at("type").AsString() == "Map") {
            ans.emplace_back(json::Dict{
                    {"map", json::Node(hand_.RenderMap())},
                    {"request_id", i.AsMap().at("id")}
            });
        }
        else if (i.AsMap().at("type").AsString() == "Bus") {
            const std::optional<BusStat> bus_stat = hand_.GetBusStat(i.AsMap().at("name").AsString());
            if (bus_stat.has_value()) {
                ans.emplace_back(json::Dict{
                        {"curvature", json::Node(bus_stat->curvature)},
                        {"request_id", i.AsMap().at("id")},
                        {"route_length", json::Node(static_cast<int>(bus_stat->route_length))},
                        {"stop_count", json::Node(static_cast<int>(bus_stat->stops_amount))},
                        {"unique_stop_count", json::Node(static_cast<int>(bus_stat->unique_stops_amount))}
                });
            }
            else {
                ans.emplace_back(json::Dict{
                        {"error_message", json::Node("not found"s)},
                        {"request_id", i.AsMap().at("id")}
                });
            }
        }
        else if (i.AsMap().at("type").AsString() == "Stop") {
            const std::optional<StopsStat> stop_stat = hand_.GetStopStat(i.AsMap().at("name").AsString());
            if (stop_stat.has_value()) {
                json::Array buses(stop_stat->buses.size());
                std::transform(stop_stat->buses.begin(), stop_stat->buses.end(), buses.begin(), [](std::string_view bus_name) {
                    return json::Node(bus_name);
                });
                ans.emplace_back(json::Dict{
                        {"buses", buses},
                        {"request_id", i.AsMap().at("id")}
                });
            }
            else {
                ans.emplace_back(json::Dict{
                        {"error_message", json::Node("not found"s)},
                        {"request_id", i.AsMap().at("id")}
                });
            }
        }
    }
    return ans;
}

JsonReader::JsonReader(transport_catalogue::TransportCatalogue &c, renderer::MapRenderer &m, const RequestHandler &h)
        : cat_(c)
        , map_(m)
        , hand_(h)
{
}
