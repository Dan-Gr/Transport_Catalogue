#pragma once

#include <algorithm>
#include <iostream>
#include <list>
#include <map>
#include <string>
#include <string_view>

#include "geo.h"

/*
 * В этом файле вы можете разместить классы/структуры, которые являются частью предметной области (domain)
 * вашего приложения и не зависят от транспортного справочника. Например Автобусные маршруты и Остановки.
 *
 * Их можно было бы разместить и в transport_catalogue.h, однако вынесение их в отдельный
 * заголовочный файл может оказаться полезным, когда дело дойдёт до визуализации карты маршрутов:
 * визуализатор карты (map_renderer) можно будет сделать независящим от транспортного справочника.
 *
 * Если структура вашего приложения не позволяет так сделать, просто оставьте этот файл пустым.
 *
 */

typedef enum { LINE, ROUND } TYPE_ROUTE;

typedef struct {
    std::list<std::string_view> routes;
    TYPE_ROUTE type_route;
} BusRouteData;

typedef struct {
    std::unordered_map<std::string_view, double> data_stops;
    geo::Coordinates coordinates;
} StopData;

class DataStop {
   public:
    DataStop() = default;

    explicit DataStop(std::string& stop_name) : stop_name_(stop_name) {}

    std::string GetNameStop();
    geo::Coordinates GetCoordinatesStop();
    double GetDistance(std::string_view other_stop);
    int GetId();

    void SetCoordinates(const geo::Coordinates& coordinates);
    void SetNearbyStops(const std::string& other_stop, double distance);
    void SetStopName(const std::string& stop_name);
    void SetId(int id);

   private:
    int id_ = 0;
    std::string stop_name_ = "";
    std::map<std::string, double> nearby_stops_;
    geo::Coordinates coordinates_ = {0, 0};
};

class DataBusRoute {
   public:
    DataBusRoute() = default;

    explicit DataBusRoute(std::string& route_name) : route_name_(route_name) {}

    std::string GetNameRoute();
    TYPE_ROUTE GetTypeRoute();
    // std::list<std::string> GetRoute();
    std::list<std::string>& GetRoute();
    int GetCoutStop();
    int GetCountUniqueStop();

    bool CheckStopInRoute(std::string_view stops);

    void SetStop(const std::string& stop);
    void SetTypeRoute(const TYPE_ROUTE type_route);
    void SetNameRoute(const std::string& route_name);

   private:
    std::string route_name_ = "";
    std::list<std::string> routes_;
    TYPE_ROUTE type_route_ = LINE;
};
