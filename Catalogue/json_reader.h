#pragma once

#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <string_view>

// #include "json.h"
#include "graph.h"
#include "json_builder.h"
#include "map_renderer.h"
#include "router.h"
#include "transport_catalogue.h"
#include "transport_router.h"

/*
 * Здесь можно разместить код наполнения транспортного справочника данными из
 * JSON, а также код обработки запросов к базе и формирование массива ответов в
 * формате JSON
 */

void Read();

typedef struct {
  int request_id;
  std::string type;
  std::string name_route;
  double curvature;
  int route_length;
  int stop_count;
  int unique_stop_count;
  std::string error_message;
} AnswerRqRoute;

typedef struct {
  int request_id;
  std::string type;
  std::string name_stop;
  std::vector<std::string> num_shuttle_bus;
  std::string error_message;
} AnswerRqStop;

typedef struct {
  int bus_wait_time = 0;
  double bus_velocity = 0.00;
} RoutingSettings;

class ReadRequests {
 public:
  explicit ReadRequests(TransportCatalogue& catalogue)
      : catalogue_(catalogue), navigator_(catalogue) {
    ParseJson();
  }

  void ParseJson();

 private:
  TransportCatalogue& catalogue_;

  navigation::Navigator navigator_;

  MapRendering map_rendering;
  AnswerRqRoute ans_rq_route;
  AnswerRqStop ans_rq_stop;
  RoutingSettings routing_settings;

  void SaveDataRoute(const json::Node& data_in);
  void SaveDataStop(const json::Node& data_in);
  void SaveRenderSettings(const json::Node& data_in);
  void SaveRoutingSettings(const json::Node& data_in);

  void ReadBaseRequests(const std::vector<json::Node>& data);
  void ReadStatRequests(const std::vector<json::Node>& data);

  void ReadRouteRequests(const json::Node& data_in);
  void ReadStopRequests(const json::Node& data_in);
  void ReadMapRequests(const json::Node& data_in);
  void ReadRouteRequestsCreateShortRoute(const json::Node& data_in);

  void GetDataRouteRequests(AnswerRqRoute& ans_rq_route);
  void GetDataStatRequests(AnswerRqStop& ans_rq_stop);
  void GetDataRenderSettings(MapRendering& map_rendering);

  std::vector<svg::Color> GetColorPalette(std::vector<json::Node> array);
  svg::Color CrateColor(std::vector<json::Node> color);

  void OutData();
  void OutDataRouteRequests(const AnswerRqRoute& ans_rq_route);
  void OutDataStatRequests(const AnswerRqStop& ans_rq_stop);
  void OutDataRender(MapRendering& map_rendering);
  void OutDataShortRoute(std::vector<navigation::NodeRoute>& data_route,
                         int id);

  void NameRoutesRender(MapRendering& map_rendering,
                        SphereProjector& sphere_projector);

  void OutError(const int request_id, const std::string_view error_message);

  std::map<std::string, geo::Coordinates> GetCoordinatesStops(
      MapRendering& map_rendering) const;
  std::vector<geo::Coordinates> GetAllCoordinatesStopsForRender(
      MapRendering& map_rendering) const;

  void InitializationNavigator();
};
