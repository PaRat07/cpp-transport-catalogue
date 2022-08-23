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
    json::Array ans;
    for (const json::Node &i : data) {
        if (i.AsMap().at("type").AsString() == "Map") {
            ans.emplace_back(json::Dict{
                    {"map", json::Node(RenderMap())},
                    {"request_id", i.AsMap().at("id")}
            });
        }
        else if (i.AsMap().at("type").AsString() == "Bus") {
            const std::optional<BusStat> bus_stat = GetBusStat(i.AsMap().at("name").AsString());
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
            const std::optional<StopsStat> stop_stat = GetStopStat(i.AsMap().at("name").AsString());
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
