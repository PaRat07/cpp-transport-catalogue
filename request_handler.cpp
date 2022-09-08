#include "request_handler.h"

using namespace std;

std::optional<BusStat> RequestHandler::GetBusStat(const std::string_view &bus_name) const {
    return cat_.GetDataForBus(bus_name);
}

std::optional<StopsStat> RequestHandler::GetStopStat(const std::string_view &stop_name) const {
    return cat_.GetDataForStop(stop_name);
}

std::string RequestHandler::RenderMap() const {
    std::ostringstream ans;
    map_.Render(ans);
    return ans.str();
}

RequestHandler::RequestHandler(const transport_catalogue::TransportCatalogue &c,
                               const renderer::MapRenderer &m)
        : cat_(c)
        , map_(m)
{

}

json::Node RequestHandler::SolveQueries(const json::Array &data) const {
    json::Builder ans;
    ans.StartArray();
    for (const json::Node &i : data) {
        if (i.AsDict().at("type").AsString() == "Map") {
            ans.StartDict()
                .Key("map").Value(json::Node(RenderMap()))
                .Key("request_id").Value(i.AsDict().at("id"))
                .EndDict();
        }
        else if (i.AsDict().at("type").AsString() == "Bus") {
            const std::optional<BusStat> bus_stat = GetBusStat(i.AsDict().at("name").AsString());
            if (bus_stat.has_value()) {
                ans.StartDict() .Key("curvature").Value(json::Node(bus_stat->curvature))
                                .Key("request_id").Value(i.AsDict().at("id"))
                                .Key("route_length").Value(json::Node(static_cast<int>(bus_stat->route_length)))
                                .Key("stop_count").Value( json::Node(static_cast<int>(bus_stat->stops_amount)))
                                .Key("unique_stop_count") .Value(json::Node(static_cast<int>(bus_stat->unique_stops_amount)))
                                .EndDict();
            }
            else {
                ans.StartDict() .Key("error_message").Value(json::Node("not found"s))
                                .Key("request_id").Value(i.AsDict().at("id"))
                                .EndDict();
            }
        }
        else if (i.AsDict().at("type").AsString() == "Stop") {
            const std::optional<StopsStat> stop_stat = GetStopStat(i.AsDict().at("name").AsString());
            if (stop_stat.has_value()) {
                json::Array buses(stop_stat->buses.size());
                std::transform(stop_stat->buses.begin(), stop_stat->buses.end(), buses.begin(), [](std::string_view bus_name) {
                    return json::Node(string(bus_name));
                });
                ans.StartDict() .Key("buses").Value(buses)
                                .Key("request_id").Value(i.AsDict().at("id"))
                                .EndDict();
            }
            else {
                ans.StartDict() .Key("error_message").Value(json::Node("not found"s))
                                .Key("request_id").Value(i.AsDict().at("id"))
                                .EndDict();
            }
        }
    }
    ans.EndArray();
    return ans.Build();
}
