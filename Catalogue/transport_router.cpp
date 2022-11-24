#include "transport_router.h"

namespace navigation {

void Navigator::SetRoutingSettings(double bus_wait_time, double bus_velocity) {
  bus_wait_time_ = bus_wait_time;
  bus_velocity_ = bus_velocity;
}

void Navigator::AddCatalogueInGraph(TransportCatalogue& catalogue) {
  catalogue_ = catalogue;
  for (auto& [name_route, data_route] : catalogue_) {
    if (data_route.GetTypeRoute() == TYPE_ROUTE::LINE) {
      SaveDataLineRouteInGraph(data_route.GetRoute(), name_route);
    } else if (data_route.GetTypeRoute() == TYPE_ROUTE::ROUND) {
      SaveDataRoundRouteInGraph(data_route.GetRoute(), name_route);
    }
  }
  status = true;
}

bool Navigator::GetStatusWork() { return status; }

std::vector<NodeRoute> Navigator::GetRoute(std::string& from, std::string& to) {
  static graph::Router<double> route(graph_);

  std::optional<graph::Router<double>::RouteInfo> data = route.BuildRoute(
      GetIdWaitStop(GetIdStop(from)), GetIdWaitStop(GetIdStop(to)));

  std::vector<NodeRoute> data_node;
  if (data.has_value()) {
    std::string buffer_name_route;
    std::string buffer_name_stop;
    for (graph::EdgeId id_edge : data.value().edges) {
      graph::Edge edge = graph_.GetEdge(id_edge);
      if (id_edge_and_name_route.at(id_edge) != "wait") {
        buffer_name_route = id_edge_and_name_route.at(id_edge);
        data_node.push_back({buffer_name_route, edge.weight, TypeNode::BUS,
                             id_edge_end_span_count.at(id_edge)});
      } else {
        data_node.push_back(
            {id_and_name_stop_.at(edge.to), edge.weight, TypeNode::WAIT, -1});
        buffer_name_stop = id_and_name_stop_.at(edge.to);
      }
    }
  }
  return data_node;
}

void Navigator::SaveDataLineRouteInGraph(std::list<std::string>& route,
                                         std::string_view name_route) {
  for (auto it = route.begin(); it != route.end(); ++it) {
    AddDataLineInGraph(it, route.end(), *it,
                       static_cast<std::string>(name_route));
  }

  for (auto it = route.rbegin(); it != route.rend(); ++it) {
    AddDataLineInGraph(it, route.rend(), *it,
                       static_cast<std::string>(name_route));
  }
}

void Navigator::SaveDataRoundRouteInGraph(std::list<std::string>& route,
                                          std::string_view name_route) {
  int size = route.size();
  std::list<std::string> route_copy = route;

  for (auto it = route.begin(); it != route.end(); ++it) {
    AddDataRoundInGraph(it, route.end(), *it,
                        static_cast<std::string>(name_route));
    --size;
    if (size == 1) {
      break;
    }
  }
}

void Navigator::AddDataRoundInGraph(
    std::list<std::string>::iterator route_begin,
    std::list<std::string>::iterator route_end, std::string_view name_stop,
    std::string name_route) {
  size_t id_stop_it = GetIdStop(name_stop);
  if (!id_stop_and_id_wait.count(id_stop_it)) {
    size_t id_wait = GetIdWaitStop(id_stop_it);
    int id_edge = graph_.AddEdge({id_wait, id_stop_it, bus_wait_time_});
    id_edge_and_name_route[id_edge] = "wait";
  }
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
      size_t id_wait_this = 0;
      if (!id_stop_and_id_wait.count(id_stop)) {
        id_wait_this = GetIdWaitStop(id_stop);
        int id_edge = graph_.AddEdge({id_wait_this, id_stop, bus_wait_time_});
        id_edge_and_name_route[id_edge] = "wait";
      }
      id_wait_this = GetIdWaitStop(id_stop);
      int id_edge =
          graph_.AddEdge({id_stop_it, id_wait_this, distance / bus_velocity_});
      id_edge_end_span_count[id_edge] = ++span_count;
      id_edge_and_name_route[id_edge] = name_route;
    }
    buffer_name = *it_name_stop_push;
    if (*it_name_stop_push == name_stop) {
      search_start = true;
    }
  }
}

int Navigator::GetIdStop(std::string_view name_stop) {
  int id = 0;
  if (name_stop_and_id_.count(static_cast<std::string>(name_stop))) {
    id = name_stop_and_id_.at(static_cast<std::string>(name_stop));
  } else {
    id = ++counter_id;
    name_stop_and_id_.insert({static_cast<std::string>(name_stop), id});
    id_and_name_stop_.insert({id, static_cast<std::string>(name_stop)});
  }
  return id;
}

int Navigator::GetIdWaitStop(int id_stop) {
  int id_wait = 0;
  if (id_stop_and_id_wait.count(id_stop)) {
    id_wait = id_stop_and_id_wait.at(id_stop);
  } else {
    id_wait = ++counter_id;
    id_stop_and_id_wait.insert({id_stop, id_wait});
  }
  return id_wait;
}

}  // namespace navigation