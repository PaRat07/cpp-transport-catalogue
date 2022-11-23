#include "serialization.h"

using namespace std;

transport_catalogue_serialize::Stop SerializeStop(const Stop &stop) {
    transport_catalogue_serialize::Stop to_serialize;
    *to_serialize.mutable_name() = stop.name;
    to_serialize.mutable_coords()->set_lng(stop.coordinates.lng);
    to_serialize.mutable_coords()->set_lat(stop.coordinates.lat);
    return to_serialize;
}

transport_catalogue_serialize::Bus SerializeBus(const Bus &bus) {
    transport_catalogue_serialize::Bus to_serialize;
    to_serialize.set_name(bus.name);
    for (const Stop *stop : bus.stops) {
        to_serialize.add_stop(stop->name);
    }
    to_serialize.set_is_roundtrip(bus.is_roundtrip);
    return to_serialize;
}

transport_catalogue_serialize::PairFromPairsOfStringsAndInt SerializeElementOfMapOfDists(const pair<const pair<const Stop *, const Stop *>, int> &element_of_map_of_dists) {
    transport_catalogue_serialize::PairOfStrings stops_serialized;
    stops_serialized.set_first(element_of_map_of_dists.first.first->name);
    stops_serialized.set_second(element_of_map_of_dists.first.second->name);
    transport_catalogue_serialize::PairFromPairsOfStringsAndInt pair;
    pair.set_dist(element_of_map_of_dists.second);
    *pair.mutable_stops() = stops_serialized;
    return pair;
}

transport_catalogue_serialize::MapSettings SerializeMapSettings(const MapSettings& settings) {
    transport_catalogue_serialize::MapSettings to_serialize;
    to_serialize.set_width(settings.width);
    to_serialize.set_height(settings.height);
    to_serialize.set_padding(settings.padding);
    to_serialize.set_line_width(settings.line_width);
    to_serialize.set_stop_radius(settings.stop_radius);
    to_serialize.set_bus_label_font_size(settings.bus_label_font_size);
    transport_catalogue_serialize::Point p;
    p.set_x(settings.bus_label_offset.x);
    p.set_y(settings.bus_label_offset.y);
    *to_serialize.mutable_bus_label_offset() = p;
    to_serialize.set_stop_label_font_size(settings.stop_label_font_size);
    p.set_x(settings.stop_label_offset.x);
    p.set_y(settings.stop_label_offset.y);
    *to_serialize.mutable_stop_label_offset() = p;
    {
        ostringstream color;
        color << settings.underlayer_color;
        to_serialize.set_underlayer_color(color.str());
        color.clear();
    }
    to_serialize.set_underlayer_width(settings.underlayer_width);
    for (const auto& i : settings.color_palette) {
        ostringstream color;
        color << i;
        to_serialize.mutable_color_palette()->Add(color.str());
    }
    return to_serialize;
}

transport_catalogue_serialize::RouteInternalData SerializeRouteInternalData(const graph::Router<double>::RouteInternalData &to_serialize) {
    transport_catalogue_serialize::RouteInternalData route_internal_data;
    route_internal_data.set_weight(to_serialize.weight);
    if (to_serialize.prev_edge) {
        route_internal_data.set_prev_edge(*to_serialize.prev_edge);
    }

    return route_internal_data;
}

transport_catalogue_serialize::OptionalRouteInternalData SerializeOptionalRouteInternalData(const std::optional<graph::Router<double>::RouteInternalData>& to_serialize) {
    transport_catalogue_serialize::OptionalRouteInternalData optional_route_internal_data;
    if (to_serialize.has_value()) {
        *optional_route_internal_data.mutable_data() = SerializeRouteInternalData(*to_serialize);
    }
    else {
        optional_route_internal_data.set_null("");
    }

    return optional_route_internal_data;
}

transport_catalogue_serialize::RepeatedOptionalRouteInternalData SerializeRepeatedOptionalRouteInternalData(const std::vector<std::optional<graph::Router<double>::RouteInternalData>> &to_serialize) {
    transport_catalogue_serialize::RepeatedOptionalRouteInternalData ans;
    for (const auto &i : to_serialize) {
        *ans.add_data() = SerializeOptionalRouteInternalData(i);
    }
    return ans;
}

transport_catalogue_serialize::Router SerializeRouter(const graph::Router<double> &to_serialize) {
    transport_catalogue_serialize::Router router;
    for (const auto &i : to_serialize.GetRoutesInternalData()) {
        *router.add_routes_internal_data() = SerializeRepeatedOptionalRouteInternalData(i);
    }
    return router;
}

transport_catalogue_serialize::Edge SerializeEdge(const graph::Edge<double> &to_serialize) {
    transport_catalogue_serialize::Edge edge;

    edge.set_weight(to_serialize.weight);
    edge.set_from(to_serialize.from);
    edge.set_to(to_serialize.to);

    return edge;
}

transport_catalogue_serialize::IncidenceList SerializeIncidenceList(graph::DirectedWeightedGraph<double>::IncidentEdgesRange range) {
    transport_catalogue_serialize::IncidenceList incidence_list;
    for (const auto &i : range) {
        incidence_list.add_list(i);
    }
    return incidence_list;
}

transport_catalogue_serialize::Graph SerializeGraph(const graph::DirectedWeightedGraph<double> &to_serialize) {
    transport_catalogue_serialize::Graph graph;
    for (int i = 0; i < to_serialize.GetEdgeCount(); ++i) {
        *graph.add_edge() = SerializeEdge(to_serialize.GetEdge(i));
    }
    for (int i = 0; i < to_serialize.GetVertexCount(); ++i) {
        *graph.add_incidence_list() = SerializeIncidenceList(to_serialize.GetIncidentEdges(i));
    }
    return graph;
}

transport_catalogue_serialize::RouterSettings SerializeRouterSettings(const RoutingSettings &to_serialize) {
    transport_catalogue_serialize::RouterSettings settings;

    settings.set_bus_wait_time(to_serialize.bus_wait_time);
    settings.set_bus_velocity(to_serialize.bus_velocity);

    return settings;
}

void Serialize(const transport_catalogue::TransportCatalogue &cat, const MapSettings &setting, const filesystem::path &p) {
    ofstream out(p, ios::binary);
    transport_catalogue_serialize::TransportCatalogue to_serialize;
    for (const Bus &bus : cat.GetBuses()) {
        to_serialize.mutable_cat()->mutable_bus()->Add(SerializeBus(bus));
        to_serialize.mutable_cat()->mutable_dists();
    }
    for (const Stop &stop : cat.GetStops()) {
        *to_serialize.mutable_cat()->add_stop() = SerializeStop(stop);
    }
    for (auto &i : cat.GetAllDists()) {
        *to_serialize.mutable_cat()->add_dists() = SerializeElementOfMapOfDists(i);
    }
    *to_serialize.mutable_graph() = SerializeGraph(*cat.GetGraphPtr());
    *to_serialize.mutable_router_settings() = SerializeRouterSettings(cat.GetRoutingSettings());
    *to_serialize.mutable_map_settings() = SerializeMapSettings(setting);
    *to_serialize.mutable_router() = SerializeRouter(cat.GetRouter());
    to_serialize.SerializeToOstream(&out);
}

tuple<string, double, double> DeserializeStop(const transport_catalogue_serialize::Stop &stop) {
    return { stop.name(), stop.coords().lat(), stop.coords().lng() };
}

tuple<string, vector<string>, bool> DeserializeBus(const transport_catalogue_serialize::Bus &bus) {
    vector<string> stops;
    for (const string &stop : bus.stop()) {
        stops.push_back(stop);
    }
    return { bus.name(), stops, bus.is_roundtrip() };
}

pair<pair<string, string>, int> DeserializeLongShit(const transport_catalogue_serialize::PairFromPairsOfStringsAndInt &shit) {
    return{ { shit.stops().first(), shit.stops().second() }, shit.dist() };
}

MapSettings DeserializeMapSettings(const transport_catalogue_serialize::MapSettings& map) {
    MapSettings to_deserialize;
    to_deserialize.width = map.width();
    to_deserialize.height = map.height();
    to_deserialize.padding = map.padding();
    to_deserialize.line_width = map.line_width();
    to_deserialize.stop_radius = map.stop_radius();
    to_deserialize.bus_label_font_size = map.bus_label_font_size();
    to_deserialize.bus_label_offset = { map.bus_label_offset().x(), map.bus_label_offset().y() };
    to_deserialize.stop_label_font_size = map.stop_label_font_size();
    to_deserialize.stop_label_offset = { map.stop_label_offset().x(), map.stop_label_offset().y() };
    to_deserialize.underlayer_color = map.underlayer_color();
    to_deserialize.underlayer_width = map.underlayer_width();
    for (std::string_view color : map.color_palette()) {
        to_deserialize.color_palette.emplace_back(std::string(color));
    }

    return to_deserialize;
}

graph::Edge<double> DeseralizeEdge(const transport_catalogue_serialize::Edge &edge) {
    return { static_cast<graph::VertexId>(edge.from()), static_cast<graph::VertexId>(edge.to()), edge.weight() };
}

graph::DirectedWeightedGraph<double>::IncidenceList DeserializeIncidenceList(const transport_catalogue_serialize::IncidenceList &incidence_list) {
    graph::DirectedWeightedGraph<double>::IncidenceList to_deserialize;
    for (const auto &i : incidence_list.list()) {
        to_deserialize.push_back(i);
    }

    return to_deserialize;
}

graph::DirectedWeightedGraph<double> DeserializeGraph(const transport_catalogue_serialize::Graph &graph) {
    graph::DirectedWeightedGraph<double> to_deserialize;

    std::vector<graph::Edge<double>> edges;
    for (const auto &i : graph.edge()) {
        edges.push_back(DeseralizeEdge(i));
    }
    to_deserialize.SetEdges(edges);

    std::vector<graph::DirectedWeightedGraph<double>::IncidenceList> incidence_lists;
    for (const auto &i : graph.incidence_list()) {
        incidence_lists.push_back(DeserializeIncidenceList(i));
    }
    to_deserialize.SetIncidenceLists(incidence_lists);

    return to_deserialize;
}

std::vector<transport_catalogue_serialize::OptionalRouteInternalData> DeserializeRepeatedOptionalRouteInternalData(const transport_catalogue_serialize::RepeatedOptionalRouteInternalData &to_deserialize) {
    return { to_deserialize.data().begin(), to_deserialize.data().end() };
}

graph::Router<double>::RouteInternalData DeserializeRouteInternalData(const transport_catalogue_serialize::RouteInternalData &to_deserialize) {
    return { to_deserialize.weight(), (to_deserialize.has_prev_edge() ? optional<int>{to_deserialize.prev_edge()} : nullopt)};
}

std::optional<graph::Router<double>::RouteInternalData> DeserializeOptionalRouteInternalData(const transport_catalogue_serialize::OptionalRouteInternalData &to_deserialize) {
    return (to_deserialize.has_data() ? std::optional<graph::Router<double>::RouteInternalData>{DeserializeRouteInternalData(to_deserialize.data())} : nullopt);
}

graph::Router<double> DeserializeRouter(const transport_catalogue_serialize::Router &router) {
    graph::Router<double> to_deserialize;
    graph::Router<double>::RoutesInternalData data;
    for (const auto &i : router.routes_internal_data()) {
        std::vector<std::optional<graph::Router<double>::RouteInternalData>> new_elem;
        for (const auto &s : DeserializeRepeatedOptionalRouteInternalData(i)) {
            new_elem.push_back(DeserializeOptionalRouteInternalData(s));
        }
        data.push_back(new_elem);
    }
    to_deserialize.SetRoutesInternalData(data);

    return to_deserialize;
}

RoutingSettings DeserializeRoutingSettings(const transport_catalogue_serialize::RouterSettings &to_deserialize) {
    RoutingSettings settings{};

    settings.bus_wait_time = to_deserialize.bus_wait_time();
    settings.bus_velocity = to_deserialize.bus_velocity();

    return settings;
}

void Deserialize(const filesystem::path &p, transport_catalogue::TransportCatalogue &cat, renderer::MapRenderer &map) {
    ifstream in(p, ios::binary);
    transport_catalogue_serialize::TransportCatalogue to_deserialize;
    to_deserialize.ParsePartialFromIstream(&in);

    cat.SetRoutingSettings(DeserializeRoutingSettings(to_deserialize.router_settings()));
    map.SetMapSettings(DeserializeMapSettings(to_deserialize.map_settings()));

    for (const auto &stop : to_deserialize.cat().stop()) {
        string name;
        double lat, lng;
        tie(name, lat, lng) = DeserializeStop(stop);
        cat.AddStop(name, lat, lng);
    }

    for (const auto &i : to_deserialize.cat().dists()) {
        auto[stops, dist] = DeserializeLongShit(i);
        cat.SetDistBetweenStops(stops.first, stops.second, dist);
    }

    for (const auto &bus : to_deserialize.cat().bus()) {
        string name;
        vector<string> stops;
        bool is_roundtrip;
        tie(name, stops, is_roundtrip) = DeserializeBus(bus);
        map.AddBus(*cat.AddBus(name, stops, is_roundtrip));
    }
    cat.SetGraph(DeserializeGraph(to_deserialize.graph()));
    cat.SetRouter({DeserializeRouter(to_deserialize.router())});
    cat.GetRouter().SetGraphPtr(cat.GetGraphPtr());
}