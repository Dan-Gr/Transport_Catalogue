#include "map_renderer.h"

/*
 * В этом файле вы можете разместить код, отвечающий за визуализацию карты
 * маршрутов в формате SVG. Визуализация маршрутов вам понадобится во второй
 * части итогового проекта. Пока можете оставить файл пустым.
 */

MonitirSize MapRendering::GetMonitorSize() { return MonitirSize_; }

double MapRendering::GetPadding() { return padding_; }

double MapRendering::GetLineWidth() { return line_width_; }

double MapRendering::GetStopRadius() { return stop_radius_; }

int MapRendering::GetBusLabelFontSize() { return bus_label_font_size_; }

Offset MapRendering::GetBusLabelOffset() { return bus_label_offset_; }

Offset MapRendering::GetStopLabelOffset() { return stop_label_offset_; }

int MapRendering::GetStopLabelFontSize() { return stop_label_font_size_; }

svg::Color MapRendering::GetColor() { return underlayer_color_; }

double MapRendering::GetUnderlayerWidth() { return underlayer_width_; }

std::vector<svg::Color> MapRendering::GetColorPalette() {
  return color_palette_;
}

std::map<std::string, std::vector<geo::Coordinates>>&
MapRendering::GetCoordinatesStopsInRoutes() {
  return coordinates_routes_;
}

std::map<std::string, geo::Coordinates>& MapRendering::GetCoordinatesStops() {
  return coordinates_stops_;
}

svg::Color MapRendering::GetNextColor() {
  if (counter_color == static_cast<int>(color_palette_.size())) {
    counter_color = 0;
  }
  return color_palette_[counter_color++];
}

void MapRendering::ResetCounterColor() { counter_color = 0; }

void MapRendering::GetDataAuxiliaryInscriptionRender(
    const SphereProjector& sphere_projector, const std::string& name,
    const geo::Coordinates& coords) {
  svg::Text obj_text;
  obj_text.SetPosition({sphere_projector(coords)});
  obj_text.SetOffset({bus_label_offset_.dx, bus_label_offset_.dy});
  obj_text.SetFontSize(bus_label_font_size_);
  obj_text.SetFontFamily("Verdana");
  obj_text.SetFontWeight("bold");
  obj_text.SetData(name);
  obj_text.SetFillColor(underlayer_color_);
  obj_text.SetStrokeColor(underlayer_color_);
  obj_text.SetStrokeWidth(underlayer_width_);
  obj_text.SetStrokeLineCap(svg::StrokeLineCap::ROUND);
  obj_text.SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);
  document.Add(obj_text);
}

void MapRendering::GetDataInscriptionRender(
    const SphereProjector& sphere_projector, const std::string& name,
    const geo::Coordinates& coords, const svg::Color& color) {
  svg::Text obj_text;
  obj_text.SetPosition({sphere_projector(coords)});
  obj_text.SetOffset({bus_label_offset_.dx, bus_label_offset_.dy});
  obj_text.SetFontSize(bus_label_font_size_);
  obj_text.SetFontFamily("Verdana");
  obj_text.SetFontWeight("bold");
  obj_text.SetData(name);
  obj_text.SetFillColor(color);
  document.Add(obj_text);
}

int MapRendering::GetLastIdRequest() {
  int ans = 0;
  if (!id_requests_.empty()) {
    ans = id_requests_.back();
  }
  return ans;
}

void MapRendering::SetMonitorSize(const double width, const double height) {
  MonitirSize_.width = width;
  MonitirSize_.height = height;
}

void MapRendering::SetPadding(const double padding) { padding_ = padding; }

void MapRendering::SetLineWidth(const double line_width) {
  line_width_ = line_width;
}

void MapRendering::SetStopRadius(const double stop_radius) {
  stop_radius_ = stop_radius;
}

void MapRendering::SetBusLabelFontSize(const int bus_label_font_size) {
  bus_label_font_size_ = bus_label_font_size;
}

void MapRendering::SetBusLabelOffset(const double dx, const double dy) {
  bus_label_offset_.dx = dx;
  bus_label_offset_.dy = dy;
}

void MapRendering::SetStopLabelOffset(const double dx, const double dy) {
  stop_label_offset_.dx = dx;
  stop_label_offset_.dy = dy;
}

void MapRendering::SetStopLabelFontSize(const int stop_label_font_size) {
  stop_label_font_size_ = stop_label_font_size;
}

void MapRendering::SetColor(svg::Color color) { underlayer_color_ = color; }

void MapRendering::SetUnderlayerWidth(const double underlayer_width) {
  underlayer_width_ = underlayer_width;
}

void MapRendering::SetColorPalette(
    const std::vector<svg::Color>& color_palette) {
  color_palette_ = color_palette;
}

void MapRendering::SetCoordinatesStopsInRoutes(
    const std::map<std::string, std::vector<geo::Coordinates>>&
        coordinates_routes) {
  coordinates_routes_ = coordinates_routes;
}

void MapRendering::SetCoordinatesStop(
    const std::map<std::string, geo::Coordinates>& coordinates_stops) {
  coordinates_stops_ = coordinates_stops;
}

void MapRendering::AddIdRequest(int id) { id_requests_.push_back(id); }

void MapRendering::LineRoutesRender(const SphereProjector& sphere_projector) {
  for (const auto& [name, coords] : coordinates_routes_) {
    svg::Polyline obj;
    for (auto& coord : coords) {
      obj.AddPoint({sphere_projector(coord)});
      obj.SetStrokeWidth(line_width_);
      obj.SetStrokeLineCap(svg::StrokeLineCap::ROUND);
      obj.SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);
    }
    if (!coords.empty()) {
      obj.SetFillColor("none");
      obj.SetStrokeColor(GetNextColor());
      document.Add(obj);
    }
  }
}

void MapRendering::NameRoutesLineRender(
    const SphereProjector& sphere_projector, const std::string& name,
    const std::vector<geo::Coordinates>& coords) {
  if (!coords.empty()) {
    svg::Color color(GetNextColor());
    GetDataAuxiliaryInscriptionRender(sphere_projector, name, coords.front());
    GetDataInscriptionRender(sphere_projector, name, coords.front(), color);
    int size = !(coords.size() % 2) ? coords.size() / 2 - 1 : coords.size() / 2;
    if (coords.front() != coords[size]) {
      GetDataAuxiliaryInscriptionRender(sphere_projector, name, coords[size]);
      GetDataInscriptionRender(sphere_projector, name, coords[size], color);
    }
  }
}

void MapRendering::NameRoutesRoundRender(
    const SphereProjector& sphere_projector, const std::string& name,
    const std::vector<geo::Coordinates>& coords) {
  if (!coords.empty()) {
    svg::Color color(GetNextColor());
    GetDataAuxiliaryInscriptionRender(sphere_projector, name, coords.front());
    GetDataInscriptionRender(sphere_projector, name, coords.front(), color);
  }
}

void MapRendering::StopsRender(const SphereProjector& sphere_projector) {
  for (const auto& [name, coord] : coordinates_stops_) {
    svg::Circle obj;
    obj.SetCenter({sphere_projector(coord)});
    obj.SetRadius(stop_radius_);
    obj.SetFillColor("white");
    document.Add(obj);
  }
}

void MapRendering::NameStopsRender(const SphereProjector& sphere_projector) {
  for (const auto& [name, coord] : coordinates_stops_) {
    svg::Text obj_auxiliary;
    obj_auxiliary.SetPosition({sphere_projector(coord)});
    obj_auxiliary.SetOffset({stop_label_offset_.dx, stop_label_offset_.dy});
    obj_auxiliary.SetFontSize(stop_label_font_size_);
    obj_auxiliary.SetFontFamily("Verdana");
    obj_auxiliary.SetData(name);
    obj_auxiliary.SetFillColor(underlayer_color_);
    obj_auxiliary.SetStrokeColor(underlayer_color_);
    obj_auxiliary.SetStrokeWidth(underlayer_width_);
    obj_auxiliary.SetStrokeLineCap(svg::StrokeLineCap::ROUND);
    obj_auxiliary.SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);
    svg::Text obj_text;
    obj_text.SetPosition({sphere_projector(coord)});
    obj_text.SetOffset({stop_label_offset_.dx, stop_label_offset_.dy});
    obj_text.SetFontSize(stop_label_font_size_);
    obj_text.SetFontFamily("Verdana");
    obj_text.SetData(name);
    obj_text.SetFillColor("black");
    document.Add(obj_auxiliary);
    document.Add(obj_text);
  }
}

void MapRendering::Rendering(std::ostream& out) { document.Render(out); }
