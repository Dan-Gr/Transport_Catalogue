#include "svg.h"

namespace svg {

using namespace std::literals;

void Object::Render(const RenderContext& context) const {
    context.RenderIndent();

    // Делегируем вывод тега своим подклассам
    RenderObject(context);

    context.out << std::endl;
}

Rgb::Rgb(uint8_t r, uint8_t g, uint8_t b) : red(r), green(g), blue(b) {}

Rgba::Rgba(uint8_t r, uint8_t g, uint8_t b, double op) : red(r), green(g), blue(b), opacity(op) {}

// ---------- PrintColor ------------------

void PrintColor::operator()(std::monostate) const { out << "none"; }

void PrintColor::operator()(std::string color) const { out << color; }

void PrintColor::operator()(Rgb color) const {
    out << "rgb(";
    out << static_cast<int>(color.red) << ",";
    out << static_cast<int>(color.green) << ",";
    out << static_cast<int>(color.blue) << ")";
}

void PrintColor::operator()(Rgba color) const {
    out << "rgba(";
    out << static_cast<int>(color.red) << ",";
    out << static_cast<int>(color.green) << ",";
    out << static_cast<int>(color.blue) << ",";
    out << static_cast<double>(color.opacity) << ")";
}

// ---------- Circle ------------------

Circle& Circle::SetCenter(Point center) {
    center_ = center;
    return *this;
}

Circle& Circle::SetRadius(double radius) {
    radius_ = radius;
    return *this;
}

void Circle::RenderObject(const RenderContext& context) const {
    auto& out = context.out;
    out << "<circle cx=\""sv << center_.x << "\" cy=\""sv << center_.y << "\" "sv;
    out << "r=\""sv << radius_ << "\""sv;
    RenderAttrs(out);
    out << "/>"sv;
}

// ---------- Polyline ------------------

Polyline& Polyline::AddPoint(Point point) {
    points_.push_back(point);
    return *this;
}

void Polyline::RenderObject(const RenderContext& context) const {
    int size = points_.size();
    auto& out = context.out;
    out << "<polyline points=\"";
    for (const auto point : points_) {
        out << point.x << "," << point.y;
        --size;
        if (size >= 1) {
            out << " ";
        }
    }
    out << "\"";
    RenderAttrs(out);
    out << "/> ";
}

// ---------- Text ------------------

// Задаёт координаты опорной точки (атрибуты x и y)
Text& Text::SetPosition(Point pos) {
    start_pos_ = pos;
    return *this;
}

// Задаёт смещение относительно опорной точки (атрибуты dx, dy)
Text& Text::SetOffset(Point offset) {
    offset_ = offset;
    return *this;
}

// Задаёт размеры шрифта (атрибут font-size)
Text& Text::SetFontSize(uint32_t size) {
    size_font_ = size;
    return *this;
}

// Задаёт название шрифта (атрибут font-family)
Text& Text::SetFontFamily(std::string font_family) {
    font_family_ = font_family;
    return *this;
}

// Задаёт толщину шрифта (атрибут font-weight)
Text& Text::SetFontWeight(std::string font_weight) {
    font_weight_ = font_weight;
    return *this;
}

// Задаёт текстовое содержимое объекта (отображается внутри тега text)
Text& Text::SetData(std::string data) {
    data_text_ = data;
    return *this;
}

void Text::RenderObject(const RenderContext& context) const {
    auto& out = context.out;
    out << "<text";
    RenderAttrs(out);
    FormFeaturesText(out);
    // RenderAttrs(out);
    out << ">";
    for (const auto sign : data_text_) {
        out << CheckSpecialSymbol(sign);
    }
    out << "</text>";
}

void Text::FormFeaturesText(std::ostream& out) const {
    out << " x=\"" << start_pos_.x << "\"";
    out << " y=\"" << start_pos_.y << "\"";
    out << " dx=\"" << offset_.x << "\"";
    out << " dy=\"" << offset_.y << "\"";
    out << " font-size=\"" << size_font_ << "\"";

    if (!font_family_.empty()) {
        out << " font-family=\"" << font_family_ << "\"";
    }
    if (!font_weight_.empty()) {
        out << " font-weight=\"" << font_weight_ << "\"";
    }
}

std::string Text::CheckSpecialSymbol(const char sign) const {
    std::string result;
    switch (sign) {
        case '"':
            result = "&quot;";
            break;
        case '\'':
            result = "&apos;";
            break;
        case '<':
            result = "&lt;";
            break;
        case '>':
            result = "&gt;";
            break;
        case '&':
            result = "&amp;";
            break;
        default:
            result.push_back(sign);
            break;
    }
    return result;
}

// ---------- Document ------------------

void Document::AddPtr(std::unique_ptr<Object>&& obj) { objects_.emplace_back(std::move(obj)); }

void Document::Render(std::ostream& out) const {
    out << "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n";
    out << "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\">\n";
    RenderContext ctx(out, 0, 2);
    for (const std::unique_ptr<Object>& obj : objects_) {
        ctx.RenderIndent();
        obj.get()->Render(ctx);
    }
    out << "</svg>"sv;
}

}  // namespace svg