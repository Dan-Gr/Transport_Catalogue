#pragma once

#include <algorithm>
#include <deque>
#include <iostream>
#include <iterator>
#include <list>
#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#include "domain.h"

class TransportCatalogue {
   public:
    TransportCatalogue() = default;

    using Iterator =
        std::unordered_map<std::string_view, DataBusRoute, std::hash<std::string_view>>::iterator;

    Iterator begin() { return data_routes_json_.begin(); }

    Iterator end() { return data_routes_json_.end(); }

    // --------- блок функций при чтении из JSON --------- начало
    void SetDataBusRoute(const DataBusRoute& data_bus_route, std::deque<std::string>::iterator name_route);
    void SetDataStop(const DataStop& data_stop, std::deque<std::string>::iterator stop_name);

    std::deque<std::string>::iterator SaveNameStop(const std::string& stop_name);
    std::deque<std::string>::iterator SaveNameRoute(const std::string& num_route);

    bool CountRoute(std::string_view num_route);
    bool CountStop(std::string_view name_stops);

    std::deque<std::string>::iterator GetIteratorNamesStop(std::string_view name_stop);
    std::deque<std::string>::iterator GetIteratorNamesRoute(std::string_view num_route);

    double GetCurvature(std::string_view num_route);

    double GetRealDistance(std::string_view num_route);
    double GetRealDistanceLine(std::string_view num_route);
    double GetRealDistanceLineInOneDirection(const std::list<std::string>& route, std::string_view& stop_one,
                                             std::string_view& stop_two);
    double GetRealDistanceRound(std::string_view num_route);
    double GetGeographicalDistance(std::string_view num_route);
    double GetDistanceNextStop(std::string_view stop_one, std::string_view stop_two);

    int GetStopCount(std::string_view num_route);
    int GetStopUniqueCount(std::string_view num_route);
    size_t GetAllStopCount() const;

    int GetCountRoutes();

    int GetIdStop(std::string_view name_stop);
    std::string GetNameStop(int id);

    std::string GetCommonRoute(int id_first_stop, int id_second_stop);
    std::string GetCommonRoute(std::string id_first_stop, std::string id_second_stop);

    std::map<std::string, std::vector<geo::Coordinates>> GetCoordinatesAllRoutes();
    std::vector<std::string> GetRoutesInStop(std::string_view name_stop);
    std::list<std::string> GetStopsInRoute(std::string_view name_route);

    TYPE_ROUTE GetTypeRoute(std::string_view name_route);

    std::string GetNameStopsInCoordinates(const geo::Coordinates& coordinate);

    // --------- блок функций при чтении из JSON --------- конец
   private:
    std::deque<std::string> save_names_stops_buses_;
    std::deque<std::string> save_names_routes;

    // --------- блок хранения данных при чтении из JSON --------- начало
    std::unordered_map<std::string_view, DataStop, std::hash<std::string_view>> data_stops_buses_json_;
    std::unordered_map<std::string_view, DataBusRoute, std::hash<std::string_view>> data_routes_json_;
    // --------- блок хранения данных при чтении из JSON --------- конец
};
