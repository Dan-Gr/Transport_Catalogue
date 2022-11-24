#include "domain.h"

/*
 * В этом файле вы можете разместить классы/структуры, которые являются частью
 * предметной области (domain) вашего приложения и не зависят от транспортного
 * справочника. Например Автобусные маршруты и Остановки.
 *
 * Их можно было бы разместить и в transport_catalogue.h, однако вынесение их в
 * отдельный заголовочный файл может оказаться полезным, когда дело дойдёт до
 * визуализации карты маршрутов: визуализатор карты (map_renderer) можно будет
 * сделать независящим от транспортного справочника.
 *
 * Если структура вашего приложения не позволяет так сделать, просто оставьте
 * этот файл пустым.
 *
 */

std::string DataStop::GetNameStop() { return stop_name_; }

geo::Coordinates DataStop::GetCoordinatesStop() { return coordinates_; }

double DataStop::GetDistance(std::string_view other_stop) {
  double result = -1;
  if (nearby_stops_.count(static_cast<std::string>(other_stop))) {
    result = nearby_stops_.at(static_cast<std::string>(other_stop));
  }
  return result;
}

int DataStop::GetId() { return id_; }

void DataStop::SetCoordinates(const geo::Coordinates& coordinates) {
  coordinates_ = coordinates;
}

void DataStop::SetNearbyStops(const std::string& other_stop, double distance) {
  if (!nearby_stops_.count(other_stop)) {
    nearby_stops_[other_stop] = distance;
  }
}

void DataStop::SetStopName(const std::string& stop_name) {
  stop_name_ = stop_name;
}

void DataStop::SetId(int id) { id_ = id; }

std::string DataBusRoute::GetNameRoute() { return route_name_; }

TYPE_ROUTE DataBusRoute::GetTypeRoute() { return type_route_; }

// std::list<std::string> DataBusRoute::GetRoute() { return routes_; }
std::list<std::string>& DataBusRoute::GetRoute() { return routes_; }

int DataBusRoute::GetCoutStop() {
  int ans = 0;
  if (GetTypeRoute() == LINE) {
    ans = routes_.size() * 2 - 1;
  } else {
    ans = routes_.size();
  }
  return ans;
}

int DataBusRoute::GetCountUniqueStop() {
  std::list<std::string_view> buf(routes_.begin(), routes_.end());
  buf.sort();
  buf.unique();
  return buf.size();
}

bool DataBusRoute::CheckStopInRoute(std::string_view stops) {
  bool ans = false;
  auto check = std::find(routes_.begin(), routes_.end(), stops);
  if (check != routes_.end()) {
    ans = true;
  }
  return ans;
}

void DataBusRoute::SetStop(const std::string& stop) { routes_.push_back(stop); }

void DataBusRoute::SetTypeRoute(TYPE_ROUTE type_route) {
  type_route_ = type_route;
}

void DataBusRoute::SetNameRoute(const std::string& route_name) {
  route_name_ = route_name;
}