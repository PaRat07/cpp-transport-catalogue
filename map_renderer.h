#pragma once

#include <algorithm>
#include <set>

#include "svg.h"
#include "geo.h"
#include "domain.h"
#include "json.h"

namespace renderer {


inline const double EPSILON = 1e-6;
bool IsZero(double value);

// Класс, проектирующий координаты в точки на экране
class SphereProjector {
public:
    SphereProjector(double max_lat, double min_lat, double max_lon, double min_lon,
                    double max_width, double max_height, double padding)
            : padding_(padding)
            , max_lat_(max_lat)
            , min_lon_(min_lon)//
    {
        // Вычисляем коэффициент масштабирования вдоль координаты x
        std::optional<double> width_zoom;
        if (!IsZero(max_lon - min_lon_)) {
            width_zoom = (max_width - 2 * padding) / (max_lon - min_lon_);
        }

        // Вычисляем коэффициент масштабирования вдоль координаты y
        std::optional<double> height_zoom;
        if (!IsZero(max_lat_ - min_lat)) {
            height_zoom = (max_height - 2 * padding) / (max_lat_ - min_lat);
        }

        if (width_zoom && height_zoom) {
            // Коэффициенты масштабирования по ширине и высоте ненулевые,
            // берём минимальный из них
            zoom_coeff_ = std::min(*width_zoom, *height_zoom);
        } else if (width_zoom) {
            // Коэффициент масштабирования по ширине ненулевой, используем его
            zoom_coeff_ = *width_zoom;
        } else if (height_zoom) {
            // Коэффициент масштабирования по высоте ненулевой, используем его
            zoom_coeff_ = *height_zoom;
        }
    }

    // Проецирует широту и долготу в координаты внутри SVG-изображения
    svg::Point operator()(geo::Coordinates coords) const;

private:
    double padding_;
    double max_lat_ = 0;
    double min_lon_ = 0;
    double zoom_coeff_ = 0;
};


// Класс для преобразования каталога в SVG
class MapRenderer {
public:
    void AddBus(const Bus &bus);

    void Render(std::ostream &out) const;
    void SetMapSettings(MapSettings s);
private:
    MapSettings settings_;
    double max_long = -180., min_long = 180., max_lat = -90., min_lat = 90.;
    std::vector<Bus> buses_;
};
}