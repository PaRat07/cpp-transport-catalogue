#include "json_reader.h"

using namespace std;

void SolveQuery(transport_catalogue::TransportCatalogue &cat, Query q, std::ostream &out) {
    std::vector<BusDataForAdd> buses_queries;
    vector<pair<pair<string, string>, int>> stop_dists;
    for (const auto &i : q.data.GetRoot().AsMap().at("base_requests").AsArray()) {
        if (i.AsMap().at("type").AsString() == "Stop") {
            cat.AddStop(i.AsMap().at("name").AsString(), i.AsMap().at("latitude").AsDouble(), i.AsMap().at("longitude").AsDouble());
            for (const auto &[name, dist] : i.AsMap().at("road_distances").AsMap()) {
                stop_dists.push_back({{i.AsMap().at("name").AsString(), name}, dist.AsInt()});
            }
        }
        else {
            vector<string> stops(i.AsMap().at("stops").AsArray().size());
            for (int s = 0; s < stops.size(); ++s) {
                stops[s] = i.AsMap().at("stops").AsArray()[s].AsString();
            }
            if (!i.AsMap().at("is_roundtrip").AsBool()) {
                const int old_size = stops.size();
                stops.resize(old_size * 2 - 1);
                std::copy(stops.rbegin() + old_size, stops.rend(), stops.begin() + old_size);
            }
            buses_queries.emplace_back(i.AsMap().at("name").AsString(), stops);
        }
    }
    for (const auto &[buses, dist] : stop_dists) {
        cat.SetDistBetweenStops(buses.first, buses.second, dist);
    }
    for (const auto &i : buses_queries) {
        cat.AddBus(i.name, i.stops);
    }
    json::Array arr;
    for (const auto &i : q.data.GetRoot().AsMap().at("stat_requests").AsArray()) {
        if (i.AsMap().at("type").AsString() == "Bus") {
            double curv;
            string bus_name;
            int length, stop_count, unique_stops_count;
            tie(bus_name, stop_count, unique_stops_count, length, curv) = cat.GetDataForBus(i.AsMap().at("name").AsString());
            if (stop_count == 0) {
                arr.emplace_back(json::Dict {
                        {"request_id",    i.AsMap().at("id")},
                        {"error_message", "not found"}
                });
            }
            else {
                arr.emplace_back(json::Dict{
                        {"curvature",         curv},
                        {"route_length",      length},
                        {"stop_count",        stop_count},
                        {"unique_stop_count", unique_stops_count},
                        {"request_id",        i.AsMap().at("id")}
                });
            }
        }
        else {
            bool successful;
            string name;
            std::set<std::string_view, std::less<>> buses;
            tie(successful, name, buses) = cat.GetDataForStop(i.AsMap().at("name").AsString());
            vector<json::Node> buses_vec(buses.size());
            std::transform(buses.begin(), buses.end(), buses_vec.begin(), [](string_view bus_name) {
                return json::Node(bus_name);
            });
            if (successful) {
                arr.emplace_back(json::Dict{
                        {"buses",      json::Array(buses_vec.begin(), buses_vec.end())},
                        {"request_id", i.AsMap().at("id")}
                });
            }
            else {
                arr.emplace_back(json::Dict {
                        {"request_id",    i.AsMap().at("id")},
                        {"error_message", "not found"}
                });
            }
        }
    }
    json::Print(json::Document(arr), out);
}

Query::Query(std::istream &in)
    : data(json::Load(in))
{

}