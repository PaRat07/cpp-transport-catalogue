#include "domain.h"

using namespace std;

Stop::Stop(Stop &&stop)
        : name(exchange(stop.name, "")), coordinates()
{
}

Stop::Stop(string_view n, double lat, double lng)
        : name(n), coordinates({lat, lng})
{
}

bool Stop::operator==(const Stop &other) const {
    return other.name == name && other.coordinates == coordinates;
}

Bus::Bus(Bus &&bus) noexcept
        : name(exchange(bus.name, ""s))
        , stops(exchange(bus.stops, {}))
        , is_roundtrip(exchange(bus.is_roundtrip, false))
{
}

Bus::Bus(string_view n, const std::vector<const Stop *> &s, bool i_r)
        : name(n)
        , stops(s)
        , is_roundtrip(i_r)
{
}

bool Bus::operator==(const Bus &other) const {
    return other.name == name && other.stops == stops && is_roundtrip == other.is_roundtrip;
}

svg::Color ParseColor(const json::Node &node) {
    if (node.IsArray()) {
        if (node.AsArray().size() == 3) {
            return svg::Rgb(node.AsArray()[0].AsInt(), node.AsArray()[1].AsInt(), node.AsArray()[2].AsInt());
        }
        else {
            return svg::Rgba(node.AsArray()[0].AsInt(), node.AsArray()[1].AsInt(), node.AsArray()[2].AsInt(), node.AsArray()[3].AsDouble());
        }
    }
    else {
        return node.AsString();
    }
}

MapSettings::MapSettings(const json::Dict &data)
    : width(data.at("width").AsDouble())
    , height(data.at("height").AsDouble())
    , padding(data.at("padding").AsDouble())
    , line_width(data.at("line_width").AsDouble())
    , stop_radius(data.at("stop_radius").AsDouble())
    , bus_label_font_size(data.at("bus_label_font_size").AsInt())
    , bus_label_offset(data.at("bus_label_offset").AsArray()[0].AsDouble(), data.at("bus_label_offset").AsArray()[1].AsDouble())
    , stop_label_font_size(data.at("stop_label_font_size").AsInt())
    , stop_label_offset(data.at("stop_label_offset").AsArray()[0].AsDouble(), data.at("stop_label_offset").AsArray()[1].AsDouble())
    , underlayer_color(ParseColor(data.at("underlayer_color")))
    , underlayer_width(data.at("underlayer_width").AsDouble())
    , color_palette(data.at("color_palette").AsArray().size())
{
    std::transform(data.at("color_palette").AsArray().begin(), data.at("color_palette").AsArray().end(), color_palette.begin(), ParseColor);
}

size_t StopHasher::operator()(const Stop &stop) const {
    return str_hasher(stop.name) + double_hasher(stop.coordinates.lat) * 37 + double_hasher(stop.coordinates.lng) * 37 * 37;
}

size_t StopHasher::operator()(const Stop *stop) const {
    return str_hasher(stop->name) + double_hasher(stop->coordinates.lat) * 37 + double_hasher(stop->coordinates.lng) * 37 * 37;
}

size_t StopPairHasher::operator()(std::pair<const Stop *, const Stop *> p) const {
    return stop_hash(p.first) + stop_hash(p.second) * 37;
}

size_t BusHasher::operator()(const Bus &bus) const {
    size_t ans = str_hasher(bus.name);
    for (size_t i = 0; i < bus.stops.size(); ++i) {
        ans += stop_hasher(*bus.stops[i]) * std::pow(37, i + 1);
    }
    return ans;
}

size_t BusHasher::operator()(const Bus *const bus) const {
    size_t ans = str_hasher(bus->name);
    for (size_t i = 0; i < bus->stops.size(); ++i) {
        ans += stop_hasher(*bus->stops[i]) * std::pow(37, i + 1);
    }
    return ans;
}

BusStat::BusStat(std::string_view n, size_t s_a, size_t u_s_a, size_t r_l, double c)
        : name(n)
        , stops_amount(s_a)
        , unique_stops_amount(u_s_a)
        , route_length(r_l)
        , curvature(c)
{

}

StopsStat::StopsStat(std::string_view n, std::set<std::string_view, less<>> b)
        :name(n)
        , buses(std::move(b))
{
}
