#pragma once

#include <list>
#include <string>
#include <string_view>
#include <unordered_map>

#include "graph.h"
#include "router.h"
#include "transport_catalogue.h"

namespace navigation {

typedef enum { WAIT, BUS } TypeNode;

typedef struct {
  std::string stop_name_or_route_name;
  double time = 0.00;
  TypeNode type;
  int span_count = 0;
} NodeRoute;

class Navigator {
 public:
  explicit Navigator(TransportCatalogue& catalogue) : catalogue_(catalogue) {
    graph::DirectedWeightedGraph<double> graph(264);
    graph_ = std::move(graph);
  }

  bool GetStatusWork();

  void SetRoutingSettings(double bus_wait_time_ = 0,
                          double bus_velocity_ = 0.00);
  void AddCatalogueInGraph(TransportCatalogue& catalogue);
  std::vector<NodeRoute> GetRoute(std::string& from, std::string& to);

 private:
  TransportCatalogue& catalogue_;
  graph::DirectedWeightedGraph<double> graph_;

  bool status = false;
  bool route_status = false;

  double bus_wait_time_ = 0;
  double bus_velocity_ = 0.00;

  int counter_id = 0;

  std::unordered_map<int, std::string> id_and_name_stop_;
  std::unordered_map<std::string, int> name_stop_and_id_;
  std::unordered_map<int, int> id_stop_and_id_wait;

  std::unordered_map<int, int> id_edge_end_span_count;

  std::unordered_map<int, std::string> id_edge_and_name_route;

  void SaveDataLineRouteInGraph(std::list<std::string>& route,
                                std::string_view name_route);
  void SaveDataRoundRouteInGraph(std::list<std::string>& route,
                                 std::string_view name_route);

  template <typename Iterator>
  void AddDataLineInGraph(Iterator route_begin, Iterator route_end,
                          std::string_view name_stop, std::string name_route);

  void AddDataRoundInGraph(std::list<std::string>::iterator route_begin,
                           std::list<std::string>::iterator route_end,
                           std::string_view name_stop, std::string name_route);

  int GetIdStop(std::string_view name_stop);
  int GetIdWaitStop(int id_stop);
};
}  // namespace navigation

template <typename Iterator>
void navigation::Navigator::AddDataLineInGraph(Iterator route_begin,
                                               Iterator route_end,
                                               std::string_view name_stop,
                                               std::string name_route) {
  size_t id_stop_it = GetIdStop(name_stop);
  size_t id_wait = GetIdWaitStop(id_stop_it);
  int id_edge = graph_.AddEdge({id_wait, id_stop_it, bus_wait_time_});
  id_edge_and_name_route[id_edge] = "wait";
  double distance = 0.00;
  std::string buffer_name = static_cast<std::string>(name_stop);
  bool search_start = false;
  int span_count = 0;
  for (auto& it_name_stop_push = route_begin; it_name_stop_push != route_end;
       ++it_name_stop_push) {
    if (search_start) {
      distance +=
          catalogue_.GetDistanceNextStop(buffer_name, *it_name_stop_push);
      size_t id_stop = GetIdStop(*it_name_stop_push);
      int id_wait_this = GetIdWaitStop(id_stop);
      int id_edge =
          graph_.AddEdge({id_stop_it, static_cast<size_t>(id_wait_this),
                          distance / bus_velocity_});
      id_edge_end_span_count[id_edge] = ++span_count;
      id_edge_and_name_route[id_edge] = name_route;
    }
    buffer_name = *it_name_stop_push;
    if (*it_name_stop_push == name_stop) {
      search_start = true;
    }
  }
}