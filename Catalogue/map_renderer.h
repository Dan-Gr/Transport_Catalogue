#pragma once

/*
 * В этом файле вы можете разместить код, отвечающий за визуализацию карты маршрутов в формате SVG.
 * Визуализация маршрутов вам понадобится во второй части итогового проекта.
 * Пока можете оставить файл пустым.
 */

#include <algorithm>
#include <array>
#include <cstdlib>
#include <iostream>
#include <map>
#include <optional>
#include <variant>
#include <vector>

#include "geo.h"
#include "svg.h"

typedef struct {
    double width;
    double height;
} MonitirSize;

typedef struct {
    double dx;
    double dy;
} Offset;

class SphereProjector {
   public:
    const double EPSILON = 1e-6;
    bool IsZero(double value) { return std::abs(value) < EPSILON; }

    // points_begin и points_end задают начало и конец интервала элементов geo::Coordinates
    template <typename PointInputIt>
    SphereProjector(PointInputIt points_begin, PointInputIt points_end, double max_width, double max_height,
                    double padding)
        : padding_(padding)  //
    {
        // Если точки поверхности сферы не заданы, вычислять нечего
        if (points_begin == points_end) {
            return;
        }

        // Находим точки с минимальной и максимальной долготой
        const auto [left_it, right_it] = std::minmax_element(
            points_begin, points_end, [](auto lhs, auto rhs) { return lhs.lng < rhs.lng; });
        min_lon_ = left_it->lng;
        const double max_lon = right_it->lng;

        // Находим точки с минимальной и максимальной широтой
        const auto [bottom_it, top_it] = std::minmax_element(
            points_begin, points_end, [](auto lhs, auto rhs) { return lhs.lat < rhs.lat; });
        const double min_lat = bottom_it->lat;
        max_lat_ = top_it->lat;

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
    svg::Point operator()(geo::Coordinates coords) const {
        return {(coords.lng - min_lon_) * zoom_coeff_ + padding_,
                (max_lat_ - coords.lat) * zoom_coeff_ + padding_};
    }

   private:
    double padding_;
    double min_lon_ = 0;
    double max_lat_ = 0;
    double zoom_coeff_ = 0;
};

class MapRendering {
   public:
    // ---------- Получение данных ------------------
    MonitirSize GetMonitorSize();
    double GetPadding();
    double GetLineWidth();
    double GetStopRadius();
    int GetBusLabelFontSize();
    Offset GetBusLabelOffset();
    Offset GetStopLabelOffset();
    int GetStopLabelFontSize();
    svg::Color GetColor();
    double GetUnderlayerWidth();
    std::vector<svg::Color> GetColorPalette();
    std::map<std::string, std::vector<geo::Coordinates>>& GetCoordinatesStopsInRoutes();
    std::map<std::string, geo::Coordinates>& GetCoordinatesStops();

    svg::Color GetNextColor();
    void ResetCounterColor();

    void GetDataAuxiliaryInscriptionRender(const SphereProjector& sphere_projector, const std::string& name,
                                           const geo::Coordinates& coords);

    void GetDataInscriptionRender(const SphereProjector& sphere_projector, const std::string& name,
                                  const geo::Coordinates& coords, const svg::Color& color);

    int GetLastIdRequest();

    // ---------- Загрузка данных ------------------
    void SetMonitorSize(const double width, const double height);
    void SetPadding(const double padding);
    void SetLineWidth(const double line_width);
    void SetStopRadius(const double stop_radius);
    void SetBusLabelFontSize(const int bus_label_font_size);
    void SetBusLabelOffset(const double dx, const double dy);
    void SetStopLabelOffset(const double dx, const double dy);
    void SetStopLabelFontSize(const int stop_label_font_size);
    void SetColor(svg::Color color);
    void SetUnderlayerWidth(const double underlayer_width);
    void SetColorPalette(const std::vector<svg::Color>& color_palette);
    void SetCoordinatesStopsInRoutes(
        const std::map<std::string, std::vector<geo::Coordinates>>& coordinates_routes);
    void SetCoordinatesStop(const std::map<std::string, geo::Coordinates>& coordinates_stops);

    void AddIdRequest(int id);

    template <typename Obj>
    void AddObjInDocument(Obj& obj);
    // ---------- Отрисовка ------------------
    void LineRoutesRender(const SphereProjector& sphere_projector);
    void NameRoutesLineRender(const SphereProjector& sphere_projector, const std::string& name,
                              const std::vector<geo::Coordinates>& coords);
    void NameRoutesRoundRender(const SphereProjector& sphere_projector, const std::string& name,
                               const std::vector<geo::Coordinates>& coords);
    void StopsRender(const SphereProjector& sphere_projector);
    void NameStopsRender(const SphereProjector& sphere_projector);

    void Rendering(std::ostream& out);

   private:
    MonitirSize MonitirSize_;
    double padding_ = 0.00;
    double line_width_ = 0.00;
    double stop_radius_ = 0.00;
    int bus_label_font_size_ = 0;
    Offset bus_label_offset_;
    Offset stop_label_offset_;
    int stop_label_font_size_ = 0;
    svg::Color underlayer_color_;
    double underlayer_width_ = 0.00;
    std::vector<svg::Color> color_palette_;

    int counter_color = 0;

    svg::Document document;

    std::map<std::string, std::vector<geo::Coordinates>> coordinates_routes_;
    std::map<std::string, geo::Coordinates> coordinates_stops_;

    std::vector<int> id_requests_;
};

template <typename Obj>
void MapRendering::AddObjInDocument(Obj& obj) {
    document.Add(obj);
}
