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
    ostringstream color;
    color << settings.underlayer_color;
    to_serialize.set_underlayer_color(color.str());
    color.clear();
    to_serialize.set_width(settings.width);
    for (const auto& i : settings.color_palette) {
        color << i;
        to_serialize.mutable_color_palette()->Add(color.str());
        color.clear();
    }
    return to_serialize;
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
    *to_serialize.mutable_map_settings() = SerializeMapSettings(setting);
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
    to_deserialize.stop_label_offset = { map.stop_label_offset().x(), map.stop_label_offset().y() };
    to_deserialize.underlayer_color = map.underlayer_color();
    to_deserialize.underlayer_width = map.underlayer_width();
    for (const std::string &color : map.color_palette()) {
        to_deserialize.color_palette.emplace_back(color);
    }

    return to_deserialize;
}

void Deserialize(const filesystem::path &p, transport_catalogue::TransportCatalogue &cat, renderer::MapRenderer &map) {
    ifstream in(p, ios::binary);
    transport_catalogue_serialize::TransportCatalogue to_deserialize;
    to_deserialize.ParsePartialFromIstream(&in);

    for (const auto &stop : to_deserialize.cat().stop()) {
        string name;
        double lat, lng;
        tie(name, lat, lng) = DeserializeStop(stop);
        cat.AddStop(name, lat, lng);
    }

    map.SetMapSettings(DeserializeMapSettings(to_deserialize.map_settings()));

    for (const auto &bus : to_deserialize.cat().bus()) {
        string name;
        vector<string> stops;
        bool is_roundtrip;
        tie(name, stops, is_roundtrip) = DeserializeBus(bus);
        map.AddBus(*cat.AddBus(name, stops, is_roundtrip));
    }

    for (const auto &i : to_deserialize.cat().dists()) {
        auto[stops, dist] = DeserializeLongShit(i);
        cat.SetDistBetweenStops(stops.first, stops.second, dist);
    }
}