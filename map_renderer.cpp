#include <iostream>
#include "map_renderer.h"

using namespace std;
/*
 * В этом файле вы можете разместить код, отвечающий за визуализацию карты маршрутов в формате SVG.
 * Визуализация маршрутов вам понадобится во второй части итогового проекта.
 * Пока можете оставить файл пустым.
 */
void renderer::MapRenderer::AddBus(const Bus &bus) {
    buses_.push_back(bus);
    for (const Stop* stop : bus.stops) {
        if (stop->coordinates.lat > max_lat) {
            max_lat = stop->coordinates.lat;
        }
        if (stop->coordinates.lat < min_lat) {
            min_lat = stop->coordinates.lat;
        }
        if (stop->coordinates.lng > max_long) {
            max_long = stop->coordinates.lng;
        }
        if (stop->coordinates.lng < min_long) {
            min_long = stop->coordinates.lng;
        }
    }
    std::sort(buses_.begin(), buses_.end(), [](const Bus& lhs, const Bus& rhs) {
        return lhs.name < rhs.name;
    });
}

renderer::MapRenderer::MapRenderer(MapSettings s)
        : settings_(s)
{
}

void renderer::MapRenderer::Render(std::ostream &out) const {
    svg::Document doc_;
    SphereProjector proj(max_lat, min_lat, max_long, min_long, settings_.width, settings_.height, settings_.padding);
    std::vector<const Stop*> stops;
    for (size_t i = 0; i < buses_.size(); ++i) {
        auto bus = std::make_unique<svg::Polyline>(svg::Polyline().SetStrokeColor(settings_.color_palette[i % settings_.color_palette.size()])
                                                                  .SetFillColor(svg::NoneColor).SetStrokeWidth(settings_.line_width)
                                                                  .SetStrokeLineCap(svg::StrokeLineCap::ROUND)
                                                                  .SetStrokeLineJoin(svg::StrokeLineJoin::ROUND));
        for (const Stop* s : buses_[i].stops) {
            stops.push_back(s);
            bus->AddPoint(proj(s->coordinates));
        }
        if (!buses_[i].is_roundtrip) {
            for (int s = buses_[i].stops.size() - 2; s >= 0; --s) {
                bus->AddPoint(proj(buses_[i].stops[s]->coordinates));
            }
        }
        doc_.AddPtr(std::move(bus));
    }
    std::sort(stops.begin(), stops.end(), [] (const Stop* lhs, const Stop* rhs) {
        return lhs->name < rhs->name;
    });
    stops.erase(std::unique(stops.begin(), stops.end()), stops.end());
    for (size_t i = 0; i < buses_.size(); ++i) {
        doc_.Add(svg::Text().SetData(buses_[i].name)
                            .SetPosition(proj(buses_[i].stops.front()->coordinates))
                            .SetFontFamily("Verdana")
                            .SetFontWeight("bold")
                            .SetOffset(settings_.bus_label_offset)
                            .SetFontSize(settings_.bus_label_font_size)
                            .SetFillColor(settings_.underlayer_color)
                            .SetStrokeColor(settings_.underlayer_color)
                            .SetStrokeWidth(settings_.underlayer_width)
                            .SetStrokeLineJoin(svg::StrokeLineJoin::ROUND)
                            .SetStrokeLineCap(svg::StrokeLineCap::ROUND));

        doc_.Add(svg::Text().SetData(buses_[i].name)
                            .SetPosition(proj(buses_[i].stops.front()->coordinates))
                            .SetFontFamily("Verdana")
                            .SetFontWeight("bold")
                            .SetOffset(settings_.bus_label_offset)
                            .SetFontSize(settings_.bus_label_font_size)
                            .SetFillColor(settings_.color_palette[i % settings_.color_palette.size()]));

        if (!buses_[i].is_roundtrip && buses_[i].stops.front() != buses_[i].stops.back()) {
            doc_.Add(svg::Text().SetData(buses_[i].name)
                             .SetPosition(proj(buses_[i].stops.back()->coordinates))
                             .SetFontFamily("Verdana")
                             .SetFontWeight("bold")
                             .SetOffset(settings_.bus_label_offset)
                             .SetFontSize(settings_.bus_label_font_size)
                             .SetFillColor(settings_.underlayer_color)
                             .SetStrokeColor(settings_.underlayer_color)
                             .SetStrokeWidth(settings_.underlayer_width)
                             .SetStrokeLineJoin(svg::StrokeLineJoin::ROUND)
                             .SetStrokeLineCap(svg::StrokeLineCap::ROUND));

            doc_.Add(svg::Text().SetData(buses_[i].name)
                             .SetPosition(proj(buses_[i].stops.back()->coordinates))
                             .SetFontFamily("Verdana")
                             .SetFontWeight("bold")
                             .SetOffset(settings_.bus_label_offset)
                             .SetFontSize(settings_.bus_label_font_size)
                             .SetFillColor(settings_.color_palette[i % settings_.color_palette.size()]));
        }
    }
    for (const Stop* s : stops) {
        doc_.Add(svg::Circle().SetCenter(proj(s->coordinates))
                         .SetRadius(settings_.stop_radius)
                         .SetFillColor("white"));
    }
    for (const Stop* s : stops) {
        doc_.Add(svg::Text().SetData(s->name)
                            .SetPosition(proj(s->coordinates))
                            .SetOffset(settings_.stop_label_offset)
                            .SetFontSize(settings_.stop_label_font_size)
                            .SetFontFamily("Verdana")
                            .SetFillColor(settings_.underlayer_color)
                            .SetStrokeColor(settings_.underlayer_color)
                            .SetStrokeWidth(settings_.underlayer_width)
                            .SetStrokeLineCap(svg::StrokeLineCap::ROUND)
                            .SetStrokeLineJoin(svg::StrokeLineJoin::ROUND));

        doc_.Add(svg::Text().SetData(s->name)
                         .SetPosition(proj(s->coordinates))
                         .SetOffset(settings_.stop_label_offset)
                         .SetFontSize(settings_.stop_label_font_size)
                         .SetFontFamily("Verdana")
                         .SetFillColor("black"));
    }
    doc_.Render(out);
}

svg::Point renderer::SphereProjector::operator()(geo::Coordinates coords) const {
    return {
            (coords.lng - min_lon_) * zoom_coeff_ + padding_,
            (max_lat_ - coords.lat) * zoom_coeff_ + padding_
    };
}

bool renderer::IsZero(double value) {
    return std::abs(value) < EPSILON;
}
