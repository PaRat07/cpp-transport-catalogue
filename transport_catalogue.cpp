#include "transport_catalogue.h"

using namespace std;

namespace transport_catalogue {
    void TransportCatalogue::SetDistBetweenStops(string_view lhs, string_view rhs, int dist) {
        dist_between_stops_[{SearchStopByName(lhs), SearchStopByName(rhs)}] = dist;
    }

    int TransportCatalogue::GetDistBetweenStops(const Stop *lhs, const Stop *rhs) const {
        return dist_between_stops_.count({lhs, rhs}) ? dist_between_stops_.at({lhs, rhs}) : dist_between_stops_.count({rhs, lhs}) ? dist_between_stops_.at({rhs, lhs}) : geo::ComputeDistance(rhs->coordinates, lhs->coordinates);
    }

    const Bus *TransportCatalogue::SearchBusByName(std::string_view name) const {
        return bus_to_name.count(name) ? bus_to_name.at(name) : nullptr;
    }

    const Stop *TransportCatalogue::SearchStopByName(std::string_view name) const {
        return stops_to_name_.count(name) ? stops_to_name_.at(name) : nullptr;
    }

    const Bus* TransportCatalogue::AddBus(std::string_view bus_name, const std::vector<std::string> &stops, bool is_roundtrip) {
        Bus bus;
        bus.name = bus_name;
        bus.is_roundtrip = is_roundtrip;
        for (const string &name: stops) {
            const auto stop = SearchStopByName(name);
            bus.stops.push_back(stop);
        }
        const Bus *bus_ptr = &buses_.emplace_back(bus);
        bus_to_name[bus_ptr->name] = bus_ptr;
        for (auto stop = bus_ptr->stops.begin(); stop != bus_ptr->stops.end(); ++stop) {
            double dist_between_stops = 0.;
            unique_buses_for_stop_[(*stop)->name].insert(bus_ptr->name);
            unique_stops_for_bus_[bus_ptr].insert(*stop);
            for (auto stop1 = stop + 1; stop1 != bus_ptr->stops.end(); ++stop1) {
                dist_between_stops += GetDistBetweenStops(*(stop1 - 1), *stop1);
                graph::EdgeId edge_id = graph_.AddEdge({graph_.GetEdge(edge_to_name_[(*stop)->name]).to, graph_.GetEdge(edge_to_name_[(*stop1)->name]).from, dist_between_stops / settings_.bus_velocity});
                router_.AddEdge(edge_id);
                info_to_edge_[edge_id] = EdgeInfo{ EdgeType::BUS, dist_between_stops / settings_.bus_velocity, string(bus_name), static_cast<int>(stop1 - stop)};
            }
        }
        if (!is_roundtrip) {
            for (auto stop = bus_ptr->stops.rbegin(); stop != bus_ptr->stops.rend(); ++stop) {
                double dist_between_stops = 0.;
                for (auto stop1 = stop + 1; stop1 != bus_ptr->stops.rend(); ++stop1) {
                    dist_between_stops += GetDistBetweenStops(*(stop1 - 1), *stop1);
                    graph::EdgeId edge_id = graph_.AddEdge({graph_.GetEdge(edge_to_name_[(*stop)->name]).to, graph_.GetEdge(edge_to_name_[(*stop1)->name]).from, dist_between_stops / settings_.bus_velocity});
                    router_.AddEdge(edge_id);
                    info_to_edge_[edge_id] = EdgeInfo{ EdgeType::BUS, dist_between_stops / settings_.bus_velocity, string(bus_name), static_cast<int>(stop1 - stop)};
                }
            }
        }

        return bus_ptr;
    }

    void TransportCatalogue::AddStop(std::string_view stop_name, double lat, double lng) {
        const Stop *stop = &stops_.emplace_back(stop_name, lat, lng);
        stops_to_name_[stop->name] = stop;
        unique_buses_for_stop_[stop->name];
        graph::VertexId first = graph_.AddVertex();
        router_.AddVertex(first);
        graph::VertexId second = graph_.AddVertex();
        router_.AddVertex(second);
        graph::EdgeId id = graph_.AddEdge({ first, second, static_cast<double>(settings_.bus_wait_time) });
        router_.AddEdge(id);
        edge_to_name_[stop->name] = id;
        info_to_edge_[id] = EdgeInfo { EdgeType::WAIT, static_cast<double>(settings_.bus_wait_time), string(stop_name), 0 };
    }

    optional<BusStat> TransportCatalogue::GetDataForBus(std::string_view bus) const {
        const Bus *bus_ptr = SearchBusByName(bus);
        if (bus_ptr == nullptr) {
            return nullopt;
        }
        double route_length = transform_reduce(bus_ptr->stops.begin(), bus_ptr->stops.end() - 1,
                                               bus_ptr->stops.begin() + 1,
                                               0.0, std::plus<>(), [](const Stop *lhs, const Stop *rhs) {
                    return ComputeDistance(lhs->coordinates, rhs->coordinates);
                });
        double real_route_length = transform_reduce(bus_ptr->stops.begin(), bus_ptr->stops.end() - 1,
                                                    bus_ptr->stops.begin() + 1,
                                                    0.0, std::plus<>(), [&](const Stop *lhs, const Stop *rhs) {
                    return GetDistBetweenStops(lhs, rhs);
                });    if (!bus_ptr->is_roundtrip) {
            real_route_length += transform_reduce(bus_ptr->stops.rbegin(), bus_ptr->stops.rend() - 1,
                                                  bus_ptr->stops.rbegin() + 1,
                                                  0.0, std::plus<>(), [&](const Stop *lhs, const Stop *rhs) {
                        return GetDistBetweenStops(lhs, rhs);
                    });;
            route_length *= 2;
        }
        return BusStat(bus, (bus_ptr->is_roundtrip ? bus_ptr->stops.size() : bus_ptr->stops.size() * 2 - 1), unique_stops_for_bus_.at(bus_ptr).size(), real_route_length,
                       real_route_length / route_length);
    }

    std::optional<StopsStat>
    TransportCatalogue::GetDataForStop(std::string_view stop_name) const {
        if (SearchStopByName(stop_name) == nullptr) {
            return nullopt;
        } else {
            return StopsStat(stop_name, unique_buses_for_stop_.at(stop_name));
        }
    }

    std::optional<RouteStats> TransportCatalogue::GetRoute(std::string_view from, std::string_view to) const {
        auto route = router_.BuildRoute(graph_.GetEdge(edge_to_name_.at(from)).from, graph_.GetEdge(edge_to_name_.at(to)).from);
        if (route.has_value()) {
            std::vector<EdgeInfo> ans(route->edges.size());
            std::transform(route->edges.begin(), route->edges.end(), ans.begin(), [&](const graph::EdgeId &id) {
                return info_to_edge_.at(id);
            });
            return { { route->weight, ans } };
        }
        else {
            return std::nullopt;
        }
    }

}