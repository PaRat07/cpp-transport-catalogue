#pragma once

#define _USE_MATH_DEFINES
#include <cmath>
#include <cstdint>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <optional>
#include <variant>

namespace svg {
    struct Rgb {
        Rgb() = default;
        Rgb(uint8_t r, uint8_t g, uint8_t b)
                : red(r)
                , green(g)
                , blue(b)
        {

        }
        uint16_t red = 0, green = 0, blue = 0;
    };

    struct Rgba {
        Rgba() = default;
        Rgba(uint8_t r, uint8_t g, uint8_t b, double o)
                : red(r)
                , green(g)
                , blue(b)
                , opacity(o)
        {

        }
        uint16_t red = 0, green = 0, blue = 0;
        double opacity = 1.0;
    };
    using Color = std::variant<std::monostate, std::string, Rgb, Rgba>;
    struct ColorPrinter {
        std::ostream &ans;
        void operator() (Rgb col);
        void operator() (Rgba col);
        void operator() (std::monostate);
        void operator() (std::string col);
    };
    inline const Color NoneColor{"none"};

    enum class StrokeLineCap {
        BUTT,
        ROUND,
        SQUARE,
    };

    enum class StrokeLineJoin {
        ARCS,
        BEVEL,
        MITER,
        MITER_CLIP,
        ROUND,
    };
} // namespace svg

// Операторы вывода в поток
std::ostream &operator<<(std::ostream &out, svg::Color col);
std::ostream &operator<<(std::ostream &out, svg::StrokeLineCap line_cap);
std::ostream &operator<<(std::ostream &out, svg::StrokeLineJoin line_join);

namespace svg {
    struct Point {
        Point() = default;
        Point(double x, double y)
                : x(x)
                , y(y) {
        }
        double x = 0;
        double y = 0;
    };

/*
* Вспомогательная структура, хранящая контекст для вывода SVG-документа с отступами.
* Хранит ссылку на поток вывода, текущее значение и шаг отступа при выводе элемента
*/
    struct RenderContext {
        RenderContext(std::ostream& out)
                : out(out) {
        }

        RenderContext(std::ostream& out, int indent_step, int indent = 0)
                : out(out)
                , indent_step(indent_step)
                , indent(indent) {
        }

        RenderContext Indented() const {
            return {out, indent_step, indent + indent_step};
        }

        void RenderIndent() const {
            for (int i = 0; i < indent; ++i) {
                out.put(' ');
            }
        }

        std::ostream& out;
        int indent_step = 0;
        int indent = 0;
    };

/*
* Абстрактный базовый класс Object служит для унифицированного хранения
* конкретных тегов SVG-документа
* Реализует паттерн "Шаблонный метод" для вывода содержимого тега
*/
    class Object {
    public:
        void Render(const RenderContext& context) const;

        virtual ~Object() = default;

    private:
        virtual void RenderObject(const RenderContext& context) const = 0;
    };

    class ObjectContainer {
    public:
        // Добавляет в svg-документ объект-наследник svg::Object
        virtual void AddPtr(std::unique_ptr<Object>&& obj) = 0;

        template <typename Obj>
        void Add(Obj obj) {
            AddPtr(std::make_unique<Obj>(std::move(obj)));
        }
    protected:
        ~ObjectContainer() = default;
    };

    class Drawable {
    public:
        virtual void Draw(svg::ObjectContainer& container) const = 0;
        virtual ~Drawable() = default;
    };

    template<typename Owner>
    class PathProps {
    public:
        Owner& SetFillColor(Color color) {
            fill_color_ = color;
            return AsOwner();
        }
        Owner& SetStrokeColor(Color color) {
            stroke_color_ = color;
            return AsOwner();
        }
        Owner& SetStrokeWidth(double width) {
            width_ = width;
            return AsOwner();
        }
        virtual Owner& SetStrokeLineCap(StrokeLineCap line_cap) {
            line_cap_ = line_cap;
            return AsOwner();
        }
        virtual Owner& SetStrokeLineJoin(StrokeLineJoin line_join) {
            line_join_ = line_join;
            return AsOwner();
        }
    protected:
        void RenderAttrs(std::ostream& out) const {
            if (!std::holds_alternative<std::monostate>(fill_color_)) {
                out << " fill=\"" << fill_color_ << "\"";
            }
            if (!std::holds_alternative<std::monostate>(stroke_color_)) {
                out << " stroke=\"" << stroke_color_ << "\"";
            }
            if (width_ != -1) {
                out << " stroke-width=\"" << width_ << "\"";
            }
            if (line_cap_.has_value()) {
                out << " stroke-linecap=\"" << line_cap_.value() << "\"";
            }
            if (line_join_.has_value()){
                out << " stroke-linejoin=\"" << line_join_.value() << "\"";
            }
        }
        std::optional<StrokeLineCap> line_cap_;
        std::optional<StrokeLineJoin> line_join_;
        double width_ = -1;
        Color fill_color_;
        Color stroke_color_;
    private:
        Owner& AsOwner() {
            return static_cast<Owner&>(*this);
        }
    };
/*
* Класс Circle моделирует элемент <circle> для отображения круга
* https://developer.mozilla.org/en-US/docs/Web/SVG/Element/circle
*/
    class Circle final : public Object, public PathProps<Circle> {
    public:
        Circle& SetCenter(Point center);
        Circle& SetRadius(double radius);
    private:
        void RenderObject(const RenderContext& context) const override;

        Point center_;
        double radius_ = 1.0;
    };

/*
* Класс Polyline моделирует элемент <polyline> для отображения ломаных линий
* https://developer.mozilla.org/en-US/docs/Web/SVG/Element/polyline
*/
    class Polyline : public Object, public PathProps<Polyline> {
    public:
        // Добавляет очередную вершину к ломаной линии
        Polyline& AddPoint(Point point);

    private:
        void RenderObject(const RenderContext& context) const override;
        std::vector<Point> points_;
    };

/*
* Класс Text моделирует элемент <text> для отображения текста
* https://developer.mozilla.org/en-US/docs/Web/SVG/Element/text
*/
    class Text : public Object, public PathProps<Text> {
    public:
        // Задаёт координаты опорной точки (атрибуты x и y)
        Text& SetPosition(Point pos);
        // Задаёт смещение относительно опорной точки (атрибуты dx, dy)
        Text& SetOffset(Point offset);
        // Задаёт размеры шрифта (атрибут font-size)
        Text& SetFontSize(uint32_t size);
        // Задаёт название шрифта (атрибут font-family)
        Text& SetFontFamily(std::string font_family);
        // Задаёт толщину шрифта (атрибут font-weight)
        Text& SetFontWeight(std::string font_weight);
        // Задаёт текстовое содержимое объекта (отображается внутри тега text)
        Text& SetData(std::string data);
        // Прочие данные и методы, необходимые для реализации элемента <text>
    private:
        void RenderObject(const RenderContext& context) const override;
        std::string data_;
        std::string font_weight_;
        std::string font_family_;
        uint32_t font_size_ = 1;
        Point offset_ = {0.0, 0.0};
        Point pos_ = {0.0, 0.0};
    };
    namespace shapes {

        class Triangle : public svg::Drawable {
        public:
            Triangle(svg::Point p1, svg::Point p2, svg::Point p3)
                    : p1_(p1)
                    , p2_(p2)
                    , p3_(p3) {
            }

            // Реализует метод Draw интерфейса svg::Drawable
            void Draw(svg::ObjectContainer& container) const override;

        private:
            svg::Point p1_, p2_, p3_;
        };

        svg::Polyline CreateStar(svg::Point center, double outer_rad, double inner_rad, int num_rays);

        class Star : public svg::Drawable {
        public:
            Star(Point center, double outer_radius, double inner_radius, int num_rays)
                    : center_(center)
                    , outer_radius_(outer_radius)
                    , inner_radius_(inner_radius)
                    , num_rays_(num_rays)
            {

            }

            void Draw(svg::ObjectContainer& container) const override;
        private:
            Point center_;
            double outer_radius_;
            double inner_radius_;
            int num_rays_;
        };

        class Snowman : public Drawable{
        public:
            Snowman(Point head_center, double head_radius)
                    : head_pos_(head_center)
                    , hear_r_(head_radius)
            {

            }

            void Draw(svg::ObjectContainer& container) const override;
        private:
            Point head_pos_;
            double hear_r_;
        };

    } // namespace shapes

    class Document : public ObjectContainer {
    public:
        // Выводит в ostream svg-представление документа
        void Render(std::ostream& out) const;
        void AddPtr(std::unique_ptr<Object> &&obj) override;
        // Прочие методы и данные, необходимые для реализации класса Document
    private:
        std::vector<std::unique_ptr<Object>> objs_;
    };

}  // namespace svg