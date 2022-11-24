#pragma once

#include <cstdint>
#include <iostream>
#include <memory>
#include <optional>
#include <string>
#include <variant>
#include <vector>

namespace svg {

struct Rgb {
    Rgb() = default;

    Rgb(uint8_t r, uint8_t g, uint8_t b);

    uint8_t red = 0;
    uint8_t green = 0;
    uint8_t blue = 0;
};

struct Rgba {
    Rgba() = default;

    Rgba(uint8_t r, uint8_t g, uint8_t b, double op);

    uint8_t red = 0;
    uint8_t green = 0;
    uint8_t blue = 0;
    double opacity = 1.0;
};

using Color = std::variant<std::monostate, std::string, Rgb, Rgba>;
inline const Color NoneColor{std::monostate()};

struct PrintColor {
    std::ostream& out;

    void operator()(std::monostate) const;
    void operator()(std::string color) const;
    void operator()(Rgb color) const;
    void operator()(Rgba color) const;
};

struct Point {
    Point() = default;
    Point(double x, double y) : x(x), y(y) {}
    double x = 0;
    double y = 0;
};

enum class StrokeLineCap {
    BUTT,
    ROUND,
    SQUARE,
};

inline std::ostream& operator<<(std::ostream& out, StrokeLineCap line_cap) {
    switch (line_cap) {
        case StrokeLineCap::BUTT:
            out << "butt";
            break;
        case StrokeLineCap::ROUND:
            out << "round";
            break;
        case StrokeLineCap::SQUARE:
            out << "square";
            break;
    }
    return out;
}

enum class StrokeLineJoin {
    ARCS,
    BEVEL,
    MITER,
    MITER_CLIP,
    ROUND,
};

inline std::ostream& operator<<(std::ostream& out, StrokeLineJoin line_join) {
    switch (line_join) {
        case StrokeLineJoin::ARCS:
            out << "arcs";
            break;
        case StrokeLineJoin::BEVEL:
            out << "bevel";
            break;
        case StrokeLineJoin::MITER:
            out << "miter";
            break;
        case StrokeLineJoin::MITER_CLIP:
            out << "miter-clip";
            break;
        case StrokeLineJoin::ROUND:
            out << "round";
            break;
    }
    return out;
}

/*
 * Вспомогательная структура, хранящая контекст для вывода SVG-документа с отступами.
 * Хранит ссылку на поток вывода, текущее значение и шаг отступа при выводе элемента
 */
struct RenderContext {
    RenderContext(std::ostream& out) : out(out) {}

    RenderContext(std::ostream& out, int indent_step, int indent = 0)
        : out(out), indent_step(indent_step), indent(indent) {}

    RenderContext Indented() const { return {out, indent_step, indent + indent_step}; }

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

template <typename Item>
class PathProps {
   public:
    Item& SetFillColor(Color color);
    Item& SetStrokeColor(Color color);
    Item& SetStrokeWidth(double width);
    Item& SetStrokeLineCap(StrokeLineCap line_cap);
    Item& SetStrokeLineJoin(StrokeLineJoin line_join);

   protected:
    void RenderAttrs(std::ostream& out) const;

   private:
    std::optional<Color> color_fill_;
    std::optional<Color> color_stroke_;
    std::optional<double> stroke_width_;
    std::optional<StrokeLineCap> line_cap_;
    std::optional<StrokeLineJoin> line_join_;
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
class Polyline final : public Object, public PathProps<Polyline> {
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
class Text final : public Object, public PathProps<Text> {
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

   private:
    Point start_pos_ = {0.0, 0.0};
    Point offset_ = {0.0, 0.0};
    uint32_t size_font_ = 1;
    std::string font_family_;
    std::string font_weight_;
    std::string data_text_;

    void RenderObject(const RenderContext& context) const override;
    void FormFeaturesText(std::ostream& out) const;
    std::string CheckSpecialSymbol(const char sign) const;
};

class ObjectContainer {
   public:
    template <typename Obj>
    void Add(Obj obj);

    virtual void AddPtr(std::unique_ptr<Object>&& obj) = 0;

    virtual ~ObjectContainer() = default;

   protected:
    std::vector<std::unique_ptr<Object>> objects_;
};

class Document : public ObjectContainer {
   public:
    template <typename Obj>
    void Add(Obj obj);

    // Добавляет в svg-документ объект-наследник svg::Object
    void AddPtr(std::unique_ptr<Object>&& obj) override;

    // Выводит в ostream svg-представление документа
    void Render(std::ostream& out) const;

    // private:
    //     std::vector<std::unique_ptr<Object>> objects_;
};

class Drawable {
   public:
    virtual void Draw(ObjectContainer& container) const = 0;

    virtual ~Drawable() = default;
};

template <typename Obj>
void Document::Add(Obj obj) {
    objects_.emplace_back(std::make_unique<Obj>(std::move(obj)));
}

template <typename Obj>
void ObjectContainer::Add(Obj obj) {
    objects_.emplace_back(std::make_unique<Obj>(std::move(obj)));
}

template <typename Item>
Item& PathProps<Item>::SetFillColor(Color color) {
    color_fill_ = color;
    return static_cast<Item&>(*this);
}

template <typename Item>
Item& PathProps<Item>::SetStrokeColor(Color color) {
    color_stroke_ = color;
    return static_cast<Item&>(*this);
    ;
}

template <typename Item>
Item& PathProps<Item>::SetStrokeWidth(double width) {
    stroke_width_ = width;
    return static_cast<Item&>(*this);
}

template <typename Item>
Item& PathProps<Item>::SetStrokeLineCap(StrokeLineCap line_cap) {
    line_cap_ = line_cap;
    return static_cast<Item&>(*this);
}

template <typename Item>
Item& PathProps<Item>::SetStrokeLineJoin(StrokeLineJoin line_join) {
    line_join_ = line_join;
    return static_cast<Item&>(*this);
}

template <typename Item>
// RenderPathProps
void PathProps<Item>::RenderAttrs(std::ostream& out) const {
    if (color_fill_.has_value()) {
        out << " fill=\"";
        std::visit(PrintColor{out}, *color_fill_);
        out << "\"";
    }
    if (color_stroke_.has_value()) {
        out << " stroke=\"";
        std::visit(PrintColor{out}, *color_stroke_);
        out << "\"";
    }
    if (stroke_width_.has_value()) {
        out << " stroke-width=\"" << *stroke_width_ << "\"";
    }
    if (line_cap_.has_value()) {
        out << " stroke-linecap=\"" << *line_cap_ << "\"";
    }
    if (line_join_.has_value()) {
        out << " stroke-linejoin=\"" << *line_join_ << "\"";
    }
}

}  // namespace svg