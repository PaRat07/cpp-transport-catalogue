#include <sstream>
#include "svg.h"

void svg::ColorPrinter::operator()(svg::Rgb col) {
    ans << "rgb(" << col.red << "," << col.green << "," << col.blue << ")";
}

void svg::ColorPrinter::operator()(svg::Rgba col) {
    ans << "rgba(" << col.red << "," << col.green << "," << col.blue << "," << col.opacity << ")";
}

void svg::ColorPrinter::operator()(std::string col) {
    ans << col;
}

void svg::ColorPrinter::operator()(std::monostate) {
    ans << "none";
}

std::ostream &operator<<(std::ostream &out, svg::Color col) {
    std::visit(svg::ColorPrinter{out}, col);
    return out;
}

std::ostream &operator<<(std::ostream &out, svg::StrokeLineCap line_cap) {
    switch (line_cap) {
        case svg::StrokeLineCap::SQUARE:
            out << "square";
            break;
        case svg::StrokeLineCap::BUTT:
            out << "butt";
            break;
        case svg::StrokeLineCap::ROUND:
            out << "round";
            break;
    }
    return out;
}
std::ostream &operator<<(std::ostream &out, svg::StrokeLineJoin line_join) {
    switch (line_join) {
        case svg::StrokeLineJoin::ARCS:
            out << "arcs";
            break;
        case svg::StrokeLineJoin::BEVEL:
            out << "bevel";
            break;
        case svg::StrokeLineJoin::MITER:
            out << "miter";
            break;
        case svg::StrokeLineJoin::MITER_CLIP:
            out << "miter-clip";
            break;
        case svg::StrokeLineJoin::ROUND:
            out << "round";
            break;
    }
    return out;
}
namespace svg {
    using namespace std::literals;


    void Object::Render(const RenderContext& context) const {
        context.RenderIndent();

        // Делегируем вывод тега своим подклассам
        RenderObject(context);

        context.out << std::endl;
    }

// ----------- Circle -------------------

    Circle& Circle::SetCenter(Point center)  {
        center_ = center;
        return *this;
    }

    Circle& Circle::SetRadius(double radius)  {
        radius_ = radius;
        return *this;
    }

    void Circle::RenderObject(const RenderContext& context) const {
        auto& out = context.out;
        out << "<circle cx=\"" << center_.x << "\" cy=\"" << center_.y << "\"";
        out << " r=\"" << radius_ << "\"";
        RenderAttrs(out);
        out << "/>"sv;
    }
// ---------- Polyline ------------------

    Polyline &Polyline::AddPoint(Point point) {
        points_.push_back(point);
        return *this;
    }

    void Polyline::RenderObject(const RenderContext &context) const {
        auto& out = context.out;
        out << "<polyline points=\"";
        bool is_first = true;
        for (const Point& p : points_) {
            if (is_first) {
                is_first = false;
            }
            else {
                out << " ";
            }
            out << p.x << "," << p.y;
        }
        out << "\"";
        RenderAttrs(out);
        out << "/>";
    }
// ------------ Text --------------------

    Text& Text::SetPosition(Point pos) {
        pos_ = pos;
        return *this;
    }

    Text& Text::SetOffset(Point offset) {
        offset_ = offset;
        return *this;
    }

    Text& Text::SetFontSize(uint32_t size) {
        font_size_ = size;
        return *this;
    }

    Text& Text::SetFontFamily(std::string font_family) {
        font_family_ = font_family;
        return *this;
    }

    Text& Text::SetFontWeight(std::string font_weight) {
        font_weight_ = font_weight;
        return *this;
    }

    Text& Text::SetData(std::string data) {
        data_ = data;
        return *this;
    }

    void Text::RenderObject(const svg::RenderContext &context) const {
        auto& out = context.out;
        out << "<text";
        RenderAttrs(out);
        out << " x=\"" << pos_.x << "\" y=\"" << pos_.y << "\" dx=\"" << offset_.x << "\" dy=\"" << offset_.y << "\" font-size=\"" << font_size_<< "\"";
        if (!font_family_.empty()) {
            out << " font-family=\"" << font_family_ << "\"";
        }
        if (!font_weight_.empty()) {
            out << " font-weight=\"" << font_weight_ << "\"";
        }
        out << ">";
        for (const char sym : data_) {
            switch (sym) {
                case '\"':
                    out << "&quot;";
                    break;
                case '<':
                    out << "&lt;";
                    break;
                case '>':
                    out << "&gt;";
                    break;
                case '&':
                    out << "&amp;";
                    break;
                case '\'':
                    out << "&apos;";
                    break;
                default:
                    out << sym;
                    break;
            }
        }
        out << "</text>";
    }
// ---------- Triangle ------------------
    void svg::shapes::Triangle::Draw(svg::ObjectContainer& container) const {
        container.Add(svg::Polyline().AddPoint(p1_).AddPoint(p2_).AddPoint(p3_).AddPoint(p1_));
    }
// ------------ Star --------------------
    svg::Polyline svg::shapes::CreateStar(svg::Point center, double outer_rad, double inner_rad, int num_rays) {
        using namespace svg;
        Polyline polyline;
        for (int i = 0; i <= num_rays; ++i) {
            double angle = 2 * M_PI * (i % num_rays) / num_rays;
            polyline.AddPoint({center.x + outer_rad * sin(angle), center.y - outer_rad * cos(angle)});
            if (i == num_rays) {
                break;
            }
            angle += M_PI / num_rays;
            polyline.AddPoint({center.x + inner_rad * sin(angle), center.y - inner_rad * cos(angle)});
        }
        return polyline;
    }

    void svg::shapes::Star::Draw(svg::ObjectContainer &container) const {
        container.Add(CreateStar(center_, outer_radius_, inner_radius_, num_rays_).SetFillColor("red").SetStrokeColor("black"));
    }
// ---------- Snowman -------------------
    void svg::shapes::Snowman::Draw(svg::ObjectContainer &container) const {
        container.Add(Circle().SetCenter({ head_pos_.x, head_pos_.y + hear_r_ * 5}).SetRadius(hear_r_ * 2).SetFillColor("rgb(240,240,240)").SetStrokeColor("black"));
        container.Add(Circle().SetCenter({ head_pos_.x, head_pos_.y + hear_r_ * 2}).SetRadius(hear_r_ * 1.5).SetFillColor("rgb(240,240,240)").SetStrokeColor("black"));
        container.Add(Circle().SetCenter(head_pos_).SetRadius(hear_r_).SetFillColor("rgb(240,240,240)").SetStrokeColor("black"));
    }
// ---------- Document ------------------
    void Document::AddPtr(std::unique_ptr<Object> &&obj) {
        objs_.push_back(std::move(obj));
    }

    void svg::Document::Render(std::ostream &out) const {
        out << "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>" << std::endl;
        out << "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\">" << std::endl;
        for (const std::unique_ptr<Object>& i : objs_) {
            out << "  ";
            i->Render(out);
        }
        out << "</svg>" << std::endl;
    }
}
// namespace svg