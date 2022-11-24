#include "json_reader.h"

/*
 * Здесь можно разместить код наполнения транспортного справочника данными из
 * JSON, а также код обработки запросов к базе и формирование массива ответов в
 * формате JSON
 */

void ReadRequests::ParseJson() {
  json::Document document = json::Load(std::cin);
  json::Node node = document.GetRoot();
  for (const auto& [name, data] : node.AsMap()) {
    if (name == "base_requests") {
      ReadBaseRequests(data.AsArray());
    } else if (name == "stat_requests") {
      ReadStatRequests(data.AsArray());
    } else if (name == "render_settings") {
      SaveRenderSettings(data);
    } else if (name == "routing_settings") {
      SaveRoutingSettings(data);
    }
  }
}

void ReadRequests::ReadBaseRequests(const std::vector<json::Node>& data) {
  for (const json::Node& data_in : data) {
    if (data_in.AsMap().at("type").AsString() == "Bus") {
      SaveDataRoute(data_in);
    } else if (data_in.AsMap().at("type").AsString() == "Stop") {
      SaveDataStop(data_in);
    }
  }
}

void ReadRequests::SaveDataRoute(const json::Node& data_in) {
  DataBusRoute bus_route;
  std::deque<std::string>::iterator buf_name_route;
  for (const auto& [word, inf] : data_in.AsMap()) {
    if (word == "name") {
      buf_name_route = catalogue_.SaveNameRoute(inf.AsString());
      bus_route.SetNameRoute(inf.AsString());
    } else if (word == "is_roundtrip") {
      if (inf.AsBool() == true) {
        bus_route.SetTypeRoute(ROUND);
      } else {
        bus_route.SetTypeRoute(LINE);
      }
    } else if (word == "stops") {
      for (const json::Node& stop : inf.AsArray()) {
        bus_route.SetStop(stop.AsString());
      }
    }
  }
  catalogue_.SetDataBusRoute(bus_route, buf_name_route);
}

void ReadRequests::SaveDataStop(const json::Node& data_in) {
  DataStop stop;
  double latitude = 0;
  double longitude = 0;
  std::deque<std::string>::iterator buf_name_stop;
  for (const auto& [word, inf] : data_in.AsMap()) {
    if (word == "name") {
      buf_name_stop = catalogue_.SaveNameStop(inf.AsString());
      stop.SetStopName(inf.AsString());
    } else if (word == "latitude") {
      latitude = inf.AsDouble();
    } else if (word == "longitude") {
      longitude = inf.AsDouble();
    } else if (word == "road_distances") {
      for (const auto& [name_stop, distance_n] : inf.AsMap()) {
        stop.SetNearbyStops(name_stop, distance_n.AsDouble());
      }
    }
  }
  stop.SetCoordinates({latitude, longitude});
  stop.SetId(catalogue_.GetAllStopCount() + 1);
  catalogue_.SetDataStop(stop, buf_name_stop);
}

void ReadRequests::ReadStatRequests(const std::vector<json::Node>& data) {
  if (!data.empty()) {
    std::cout << "[" << std::endl;
    int fl = 0;
    for (const json::Node& data_in : data) {
      if (fl) {
        std::cout << "," << std::endl;
      }
      if (data_in.AsMap().at("type").AsString() == "Bus") {
        ReadRouteRequests(data_in);
      } else if (data_in.AsMap().at("type").AsString() == "Stop") {
        ReadStopRequests(data_in);
      } else if (data_in.AsMap().at("type").AsString() == "Map") {
        ReadMapRequests(data_in);
      } else if (data_in.AsMap().at("type").AsString() == "Route") {
        ReadRouteRequestsCreateShortRoute(data_in);
      }
      fl = 1;
    }
    std::cout << std::endl;
    std::cout << "]";
  }
}

void ReadRequests::ReadRouteRequests(const json::Node& data_in) {
  for (const auto& [word, inf] : data_in.AsMap()) {
    if (word == "id") {
      ans_rq_route.request_id = inf.AsDouble();
    } else if (word == "name") {
      ans_rq_route.name_route = inf.AsString();
    }
  }
  GetDataRouteRequests(ans_rq_route);
}

void ReadRequests::GetDataRouteRequests(AnswerRqRoute& ans_rq_route) {
  if (catalogue_.CountRoute(ans_rq_route.name_route)) {
    ans_rq_route.curvature = catalogue_.GetCurvature(ans_rq_route.name_route);
    ans_rq_route.route_length =
        catalogue_.GetRealDistance(ans_rq_route.name_route);
    ans_rq_route.stop_count = catalogue_.GetStopCount(ans_rq_route.name_route);
    ans_rq_route.unique_stop_count =
        catalogue_.GetStopUniqueCount(ans_rq_route.name_route);
    OutDataRouteRequests(ans_rq_route);
  } else {
    ans_rq_route.error_message = "\"not found\"";
    OutError(ans_rq_route.request_id, ans_rq_route.error_message);
  }
}

void ReadRequests::OutDataRouteRequests(const AnswerRqRoute& ans_rq_route) {
  json::Print(json::Document{json::Builder{}
                                 .StartDict()
                                 .Key("curvature")
                                 .Value(ans_rq_route.curvature)
                                 .Key("request_id")
                                 .Value(ans_rq_route.request_id)
                                 .Key("route_length")
                                 .Value(ans_rq_route.route_length)
                                 .Key("stop_count")
                                 .Value(ans_rq_route.stop_count)
                                 .Key("unique_stop_count")
                                 .Value(ans_rq_route.unique_stop_count)
                                 .EndDict()
                                 .Build()},
              std::cout);
}

void ReadRequests::ReadStopRequests(const json::Node& data_in) {
  for (const auto& [word, inf] : data_in.AsMap()) {
    if (word == "id") {
      ans_rq_stop.request_id = inf.AsDouble();
    } else if (word == "name") {
      ans_rq_stop.name_stop = inf.AsString();
    }
  }
  GetDataStatRequests(ans_rq_stop);
}

void ReadRequests::GetDataStatRequests(AnswerRqStop& ans_rq_stop) {
  if (catalogue_.CountStop(ans_rq_stop.name_stop)) {
    ans_rq_stop.num_shuttle_bus =
        catalogue_.GetRoutesInStop(ans_rq_stop.name_stop);
    std::sort(ans_rq_stop.num_shuttle_bus.begin(),
              ans_rq_stop.num_shuttle_bus.end());
    OutDataStatRequests(ans_rq_stop);
  } else {
    ans_rq_stop.error_message = "\"not found\"";
    OutError(ans_rq_stop.request_id, ans_rq_stop.error_message);
  }
}

void ReadRequests::OutDataStatRequests(const AnswerRqStop& ans_rq_stop) {
  std::vector<json::Node> buffer;
  for (const std::string& route : ans_rq_stop.num_shuttle_bus) {
    buffer.push_back(json::Node(route));
  }
  json::Print(json::Document{json::Builder{}
                                 .StartDict()
                                 .Key("buses")
                                 .Value(buffer)
                                 .Key("request_id")
                                 .Value(ans_rq_stop.request_id)
                                 .EndDict()
                                 .Build()},
              std::cout);
}

void ReadRequests::OutError(const int request_id,
                            const std::string_view error_message) {
  std::cout << "    {" << std::endl;
  std::cout << "        \"request_id\": " << request_id << "," << std::endl;
  std::cout << "        \"error_message\": " << error_message << std::endl;
  std::cout << "    }";
}

void ReadRequests::ReadMapRequests(const json::Node& data_in) {
  for (const auto& [word, inf] : data_in.AsMap()) {
    if (word == "id") {
      map_rendering.AddIdRequest(inf.AsDouble());
    };
  }
  OutData();
}

void ReadRequests::SaveRenderSettings(const json::Node& data_in) {
  MonitirSize monitor_size = {0, 0};
  for (const auto& [word, inf] : data_in.AsMap()) {
    if (word == "bus_label_font_size") {
      map_rendering.SetBusLabelFontSize(inf.AsDouble());
    } else if (word == "bus_label_offset") {
      map_rendering.SetBusLabelOffset(inf.AsArray().front().AsDouble(),
                                      inf.AsArray().back().AsDouble());
    } else if (word == "color_palette") {
      map_rendering.SetColorPalette(GetColorPalette(inf.AsArray()));
    } else if (word == "height") {
      monitor_size.height = inf.AsDouble();
    } else if (word == "width") {
      monitor_size.width = inf.AsDouble();
    } else if (word == "line_width") {
      map_rendering.SetLineWidth(inf.AsDouble());
    } else if (word == "padding") {
      map_rendering.SetPadding(inf.AsDouble());
    } else if (word == "stop_label_font_size") {
      map_rendering.SetStopLabelFontSize(inf.AsDouble());
    } else if (word == "stop_label_offset") {
      map_rendering.SetStopLabelOffset(inf.AsArray().front().AsDouble(),
                                       inf.AsArray().back().AsDouble());
    } else if (word == "stop_radius") {
      map_rendering.SetStopRadius(inf.AsDouble());
    } else if (word == "underlayer_color") {
      if (inf.IsString()) {
        svg::Color col(inf.AsString());
        map_rendering.SetColor(col);
      } else if (inf.IsArray()) {
        map_rendering.SetColor(CrateColor(inf.AsArray()));
      }
    } else if (word == "underlayer_width") {
      map_rendering.SetUnderlayerWidth(inf.AsDouble());
    }
    map_rendering.SetMonitorSize(monitor_size.width, monitor_size.height);
  }
  GetDataRenderSettings(map_rendering);
}

std::vector<svg::Color> ReadRequests::GetColorPalette(
    std::vector<json::Node> array) {
  std::vector<svg::Color> palette;
  palette.reserve(array.size());
  for (const json::Node& col : array) {
    if (col.IsString()) {
      palette.push_back(col.AsString());
    } else if (col.IsArray()) {
      palette.push_back(CrateColor(col.AsArray()));
    }
  }
  return palette;
}

svg::Color ReadRequests::CrateColor(std::vector<json::Node> color) {
  svg::Color result;
  if (color.size() == 3) {
    result.emplace<svg::Rgb>(color[0].AsDouble(), color[1].AsDouble(),
                             color[2].AsDouble());
  } else if (color.size() == 4) {
    result.emplace<svg::Rgba>(color[0].AsDouble(), color[1].AsDouble(),
                              color[2].AsDouble(), color[3].AsDouble());
  }
  return result;
}

void ReadRequests::GetDataRenderSettings(MapRendering& map_rendering) {
  map_rendering.SetCoordinatesStopsInRoutes(
      catalogue_.GetCoordinatesAllRoutes());
  map_rendering.SetCoordinatesStop(GetCoordinatesStops(map_rendering));
  std::vector<geo::Coordinates> all_coord =
      GetAllCoordinatesStopsForRender(map_rendering);

  SphereProjector sphere_projector(
      all_coord.begin(), all_coord.end(), map_rendering.GetMonitorSize().width,
      map_rendering.GetMonitorSize().height, map_rendering.GetPadding());

  map_rendering.LineRoutesRender(sphere_projector);
  map_rendering.ResetCounterColor();
  NameRoutesRender(map_rendering, sphere_projector);
  map_rendering.StopsRender(sphere_projector);
  map_rendering.NameStopsRender(sphere_projector);
}

void ReadRequests::NameRoutesRender(MapRendering& map_rendering,
                                    SphereProjector& sphere_projector) {
  for (const auto& [name, coords] :
       map_rendering.GetCoordinatesStopsInRoutes()) {
    if (catalogue_.GetTypeRoute(name) == LINE) {
      map_rendering.NameRoutesLineRender(sphere_projector, name, coords);
    } else {
      map_rendering.NameRoutesRoundRender(sphere_projector, name, coords);
    }
  }
}

void ReadRequests::OutDataRender(MapRendering& map_rendering) {
  map_rendering.Rendering(std::cout);
}

std::map<std::string, geo::Coordinates> ReadRequests::GetCoordinatesStops(
    MapRendering& map_rendering) const {
  std::map<std::string, geo::Coordinates> coordinates_stops;
  for (const auto& [name, coords] :
       map_rendering.GetCoordinatesStopsInRoutes()) {
    for (const auto& coord : coords) {
      coordinates_stops.insert(
          {catalogue_.GetNameStopsInCoordinates(coord), coord});
    }
  }
  return coordinates_stops;
}

std::vector<geo::Coordinates> ReadRequests::GetAllCoordinatesStopsForRender(
    MapRendering& map_rendering) const {
  std::vector<geo::Coordinates> all_coord;
  for (auto& data : map_rendering.GetCoordinatesStopsInRoutes()) {
    all_coord.insert(all_coord.begin(), data.second.begin(), data.second.end());
  }
  return all_coord;
}

void ReadRequests::OutData() {
  std::ostringstream out;
  map_rendering.Rendering(out);

  json::Print(json::Document{json::Builder{}
                                 .StartDict()
                                 .Key("map")
                                 .Value(out.str())
                                 .Key("request_id")
                                 .Value(map_rendering.GetLastIdRequest())
                                 .EndDict()
                                 .Build()},
              std::cout);
}

void ReadRequests::SaveRoutingSettings(const json::Node& data_in) {
  routing_settings.bus_wait_time = data_in.AsMap().at("bus_wait_time").AsInt();
  routing_settings.bus_velocity =
      data_in.AsMap().at("bus_velocity").AsDouble() * 1000 / 60;
}

void ReadRequests::InitializationNavigator() {
  navigator_.SetRoutingSettings(routing_settings.bus_wait_time,
                                routing_settings.bus_velocity);
  navigator_.AddCatalogueInGraph(catalogue_);
}

void ReadRequests::ReadRouteRequestsCreateShortRoute(
    const json::Node& data_in) {
  int request = data_in.AsMap().at("id").AsInt();
  std::string from = data_in.AsMap().at("from").AsString();
  std::string to = data_in.AsMap().at("to").AsString();
  if (!navigator_.GetStatusWork()) {
    InitializationNavigator();
  }
  std::vector<navigation::NodeRoute> data_route = navigator_.GetRoute(from, to);

  if (from != to) {
    double all_time = 0;
    for (auto& check : data_route) {
      all_time += check.time;
    }
    if (!data_route.empty()) {
      OutDataShortRoute(data_route, request);
    } else {
      OutError(request, "\"not found\"");
    }
  } else {
    json::Builder builder;
    builder.StartDict();
    builder.Key("items").StartArray();
    double total_time = 0.00;
    builder.EndArray();
    builder.Key("request_id").Value(request);
    builder.Key("total_time").Value(total_time);
    builder.EndDict();
    json::Print(json::Document{builder.Build()}, std::cout);
  }
}

void ReadRequests::OutDataShortRoute(
    std::vector<navigation::NodeRoute>& data_route, int id) {
  json::Builder builder;
  builder.StartDict();
  builder.Key("items").StartArray();
  double total_time = 0.00;
  for (auto& data : data_route) {
    total_time += data.time;
    builder.StartDict();
    if (data.type == navigation::TypeNode::BUS) {
      builder.Key("bus").Value(data.stop_name_or_route_name);
      builder.Key("span_count").Value(data.span_count);
      builder.Key("time").Value(data.time);
      builder.Key("type").Value("Bus");
    } else {
      builder.Key("stop_name").Value(data.stop_name_or_route_name);
      builder.Key("time").Value(data.time);
      builder.Key("type").Value("Wait");
    }
    builder.EndDict();
  }
  builder.EndArray();
  builder.Key("request_id").Value(id);
  builder.Key("total_time").Value(total_time);
  builder.EndDict();
  json::Print(json::Document{builder.Build()}, std::cout);
}

// std::vector<NodeShortRoute>
// ReadRequests::GetResultDataShortRoute(std::string& from, std::string& to) {
//     std::map<std::string, int> name_stops_and_id;
//     std::map<int, std::string> id_edge_and_name_route;
//     std::map<std::string, int> name_stops_ans_wait_id;
//     std::vector<NodeShortRoute> result;
//     std::vector<NodeShortRoute> data_short_route_all_route =
//     DataShortRouteWhithAllRoutes(
//         name_stops_and_id, id_edge_and_name_route, name_stops_ans_wait_id,
//         from, to);
//     double all_time = 0;
//     for (auto& check : data_short_route_all_route) {
//         all_time += check.time;
//     }
//     std::vector<NodeShortRoute> data_short_route_one_route =
//     DataShortRouteWhithOneRoute(from, to); double one_time = 0; for (auto&
//     check : data_short_route_one_route) {
//         one_time += check.time;
//     }
//     if (!all_time) {
//         result = data_short_route_one_route;
//     }
//     if (!one_time) {
//         result = data_short_route_all_route;
//     }
//     if (all_time && all_time < one_time) {
//         result = data_short_route_all_route;
//     } else if (one_time && one_time < all_time) {
//         result = data_short_route_one_route;
//     } else if (one_time == all_time) {
//         result = data_short_route_one_route;
//     }
//     return result;
// }

// std::vector<NodeShortRoute> ReadRequests::DataShortRouteWhithAllRoutes(
//     std::map<std::string, int>& name_stops_and_id, std::map<int, std::string>
//     id_edge_and_name_route, std::map<std::string, int>&
//     name_stops_ans_wait_id, std::string& from, std::string& to) {
//     graph::DirectedWeightedGraph<double> graph_whith_all_route =
//         CreateGraphWhithAllRoute(name_stops_and_id, id_edge_and_name_route,
//         name_stops_ans_wait_id);
//     graph::Router router(graph_whith_all_route);
//     std::optional<graph::Router<double>::RouteInfo> data_router =
//         router.BuildRoute(name_stops_and_id.at(from),
//         name_stops_and_id.at(to));
//     std::vector<NodeShortRoute> data_short_route = GetDataShortRoute(
//         graph_whith_all_route, data_router, name_stops_and_id,
//         id_edge_and_name_route, from);
//     return data_short_route;
// }

// std::vector<NodeShortRoute>
// ReadRequests::DataShortRouteWhithOneRoute(std::string& from, std::string& to)
// {
//     std::map<std::string, int> name_stops_and_id;
//     std::map<int, std::string> id_edge_and_name_route;
//     std::map<std::string, int> name_stops_ans_wait_id;
//     std::vector<NodeShortRoute> data_short_route;
//     std::string common_route = catalogue_.GetCommonRoute(from, to);
//     if (!common_route.empty()) {
//         std::list<std::string> route =
//         catalogue_.GetStopsInRoute(common_route);
//         graph::DirectedWeightedGraph<double>
//         graph(catalogue_.GetAllStopCount() * 2 + 1); int counter = 0; if
//         (catalogue_.GetTypeRoute(common_route) == TYPE_ROUTE::LINE) {
//             SaveDataLineRouteInGraph(route, graph, name_stops_and_id,
//             name_stops_ans_wait_id,
//                                      id_edge_and_name_route, counter,
//                                      common_route);
//         } else {
//             SaveDataRoundRouteInGraph(route, graph, name_stops_and_id,
//             name_stops_ans_wait_id,
//                                       id_edge_and_name_route, counter,
//                                       common_route);
//         }
//         graph::Router router(graph);
//         std::optional<graph::Router<double>::RouteInfo> data_router =
//             router.BuildRoute(name_stops_and_id.at(from),
//             name_stops_and_id.at(to));
//         data_short_route =
//             GetDataShortRoute(graph, data_router, name_stops_and_id,
//             id_edge_and_name_route, from);
//     }
//     return data_short_route;
// }

// graph::DirectedWeightedGraph<double> ReadRequests::CreateGraphWhithAllRoute(
//     std::map<std::string, int>& name_stops_and_id, std::map<int,
//     std::string>& id_edge_and_name_route, std::map<std::string, int>&
//     name_stops_ans_wait_id) { graph::DirectedWeightedGraph<double>
//     graph(catalogue_.GetAllStopCount() * 2 + 1); int counter_for_id_stops =
//     0; for (auto& [name_route, data_route] : catalogue_) {
//         std::list<std::string> route = data_route.GetRoute();
//         if (data_route.GetTypeRoute() == TYPE_ROUTE::LINE) {
//             SaveDataLineRouteInGraph(route, graph, name_stops_and_id,
//             name_stops_ans_wait_id,
//                                      id_edge_and_name_route,
//                                      counter_for_id_stops,
//                                      static_cast<std::string>(name_route));
//         } else if (data_route.GetTypeRoute() == TYPE_ROUTE::ROUND) {
//             SaveDataRoundRouteInGraph(route, graph, name_stops_and_id,
//             name_stops_ans_wait_id,
//                                       id_edge_and_name_route,
//                                       counter_for_id_stops,
//                                       static_cast<std::string>(name_route));
//         }
//     }
//     return graph;
// }

// std::vector<NodeShortRoute> ReadRequests::GetDataShortRoute(
//     graph::DirectedWeightedGraph<double>& graph,
//     std::optional<graph::Router<double>::RouteInfo>& data_router,
//     std::map<std::string, int>& name_stops_and_id, std::map<int,
//     std::string>& id_edge_and_name_route, std::string& from) {
//     std::vector<NodeShortRoute> data_short_route; std::string
//     buffer_name_route; std::string name_stop; std::string buffer_name_stop;
//     int span_count = 0; double time = 0; double buffer_we = 0; if
//     (data_router.has_value()) {
//         data_short_route.push_back(NodeShortRoute{from,
//         static_cast<double>(routing_settings.bus_wait_time),
//                                                   TypeNode::WAIT,
//                                                   span_count});
//         for (auto& id_edge : data_router.value().edges) {
//             graph::Edge edge = graph.GetEdge(id_edge);
//             // std::cout << "from: " << edge.from << std::endl;
//             // std::cout << "to: " << edge.to << std::endl;
//             if (!buffer_name_route.empty()) {
//                 for (auto& [name, id] : name_stops_and_id) {
//                     if (static_cast<size_t>(id) == edge.from) {
//                         name_stop = name;
//                         break;
//                     }
//                 }
//                 // std::cout << "name stop = " << name_stop << std::endl;
//                 // std::cout << "buffer_name_stop = " << buffer_name_stop <<
//                 std::endl; if (id_edge_and_name_route.count(id_edge)) {
//                     buffer_we = edge.weight;
//                     if (CheckFinalStop(name_stop) ||
//                         CheckTransfer(name_stop, buffer_name_stop,
//                         buffer_name_route)) { data_short_route.push_back(
//                             NodeShortRoute{buffer_name_route, time,
//                             TypeNode::BUS, span_count});
//                         data_short_route.push_back(
//                             NodeShortRoute{name_stop,
//                             static_cast<double>(routing_settings.bus_wait_time),
//                                            TypeNode::WAIT, span_count});
//                         span_count = 1;
//                         time = edge.weight;
//                     } else if (buffer_name_route ==
//                     id_edge_and_name_route.at(id_edge)) {
//                         ++span_count;
//                         time += edge.weight;
//                     } else {
//                         data_short_route.push_back(
//                             NodeShortRoute{buffer_name_route, time,
//                             TypeNode::BUS, span_count});
//                         data_short_route.push_back(
//                             NodeShortRoute{name_stop,
//                             static_cast<double>(routing_settings.bus_wait_time),
//                                            TypeNode::WAIT, span_count});
//                         span_count = 1;
//                         time = edge.weight;
//                     }
//                 }
//             } else {
//                 ++span_count;
//                 time += edge.weight;
//             }
//             if (id_edge_and_name_route.count(id_edge)) {
//                 buffer_name_route = id_edge_and_name_route.at(id_edge);
//             }
//             for (auto& [name, id] : name_stops_and_id) {
//                 if (static_cast<size_t>(id) == edge.to) {
//                     buffer_name_stop = name;
//                     break;
//                 }
//             }
//             // buffer_name_stop.clear();
//         }
//         if (CheckTransfer(name_stop, buffer_name_stop, buffer_name_route)) {
//             data_short_route.push_back(
//                 NodeShortRoute{buffer_name_route, time - buffer_we,
//                 TypeNode::BUS, span_count - 1});
//             data_short_route.push_back(NodeShortRoute{
//                 name_stop,
//                 static_cast<double>(routing_settings.bus_wait_time),
//                 TypeNode::WAIT, -1});
//             span_count = 1;
//             time = buffer_we;
//         }
//         data_short_route.push_back(NodeShortRoute{buffer_name_route, time,
//         TypeNode::BUS, span_count});
//     }
//     return data_short_route;
// }

// bool ReadRequests::CheckFinalStop(std::string& name_stop) {
//     bool result = false;
//     for (auto& [name, data_route] : catalogue_) {
//         if (data_route.GetTypeRoute() == TYPE_ROUTE::ROUND &&
//         data_route.GetRoute().back() == name_stop) {
//             result = true;
//             break;
//         }
//     }
//     return result;
// }

// bool ReadRequests::CheckTransfer(std::string buffer_name_stop, std::string
// name_stop,
//                                  std::string name_route) {
//     bool result = false;
//     // std::cout << "name_stop = " << name_stop << std::endl;
//     // std::cout << "buffer_name_stop = " << buffer_name_stop << std::endl;
//     if (!buffer_name_stop.empty() && !name_stop.empty() && buffer_name_stop
//     != name_stop) {
//         if (catalogue_.GetTypeRoute(name_route) == TYPE_ROUTE::ROUND) {
//             bool search_buffer = false;
//             for (auto name : catalogue_.GetStopsInRoute(name_route)) {
//                 if (search_buffer) {
//                     if (name != name_stop) {
//                         result = true;
//                         break;
//                     } else {
//                         break;
//                     }
//                 }
//                 if (name == buffer_name_stop) {
//                     search_buffer = true;
//                 }
//             }
//         }
//     }
//     return result;
// }

// void ReadRequests::SaveDataLineRouteInGraph(std::list<std::string>& route,
//                                             graph::DirectedWeightedGraph<double>&
//                                             graph, std::map<std::string,
//                                             int>& name_stops_and_id,
//                                             std::map<std::string, int>&
//                                             name_stops_ans_wait_id,
//                                             std::map<int, std::string>&
//                                             id_edge_and_name_route, int&
//                                             counter_for_id_stops, std::string
//                                             name_route) {
//     std::string previous_name_stop;
//     int previous_id_stop = 0;
//     int id_stop = 0;
//     int id_wait = 0;
//     int id_edge = 0;
//     for (const auto& name_stop : route) {
//         if (!previous_name_stop.empty()) {
//             id_stop = GetNextId(name_stops_and_id, name_stop,
//             counter_for_id_stops); id_wait =
//             GetNextId(name_stops_ans_wait_id, name_stop,
//             counter_for_id_stops); double distance =
//             GetDistance(previous_name_stop, name_stop); id_edge =
//             graph.AddEdge(graph::Edge<double>{static_cast<size_t>(previous_id_stop),
//                                                         static_cast<size_t>(id_wait),
//                                                         distance});
//             id_edge_and_name_route[id_edge] = name_route;
//             id_edge =
//                 graph.AddEdge(graph::Edge<double>{static_cast<size_t>(id_wait),
//                 static_cast<size_t>(id_stop),
//                                                   static_cast<double>(routing_settings.bus_wait_time)});
//             distance = GetDistance(name_stop, previous_name_stop);
//             id_edge = graph.AddEdge(graph::Edge<double>{
//                 static_cast<size_t>(id_stop),
//                 static_cast<size_t>(GetNextId(name_stops_ans_wait_id,
//                 name_stop, counter_for_id_stops)), distance});
//             id_edge_and_name_route[id_edge] = name_route;

//         } else {
//             id_wait = GetNextId(name_stops_ans_wait_id, name_stop,
//             counter_for_id_stops); id_stop = GetNextId(name_stops_and_id,
//             name_stop, counter_for_id_stops); id_edge =
//                 graph.AddEdge(graph::Edge<double>{static_cast<size_t>(id_wait),
//                 static_cast<size_t>(id_stop),
//                                                   static_cast<double>(routing_settings.bus_wait_time)});
//         }
//         previous_name_stop = name_stop;
//         previous_id_stop = id_stop;
//     }
// }

// void ReadRequests::SaveDataRoundRouteInGraph(std::list<std::string>& route,
//                                              graph::DirectedWeightedGraph<double>&
//                                              graph, std::map<std::string,
//                                              int>& name_stops_and_id,
//                                              std::map<std::string, int>&
//                                              name_stops_ans_wait_id,
//                                              std::map<int, std::string>&
//                                              id_edge_and_name_route, int&
//                                              counter_for_id_stops,
//                                              std::string name_route) {
//     std::string previous_name_stop;
//     int previous_id_stop = 0;
//     int id_stop = 0;
//     int id_wait = 0;
//     int id_edge = 0;
//     for (const auto& name_stop : route) {
//         if (!previous_name_stop.empty()) {
//             id_stop = GetNextId(name_stops_and_id, name_stop,
//             counter_for_id_stops); id_wait =
//             GetNextId(name_stops_ans_wait_id, name_stop,
//             counter_for_id_stops); double distance =
//             GetDistance(previous_name_stop, name_stop); id_edge =
//             graph.AddEdge(graph::Edge<double>{static_cast<size_t>(previous_id_stop),
//                                                         static_cast<size_t>(id_wait),
//                                                         distance});
//             id_edge_and_name_route[id_edge] = name_route;
//             id_edge =
//                 graph.AddEdge(graph::Edge<double>{static_cast<size_t>(id_wait),
//                 static_cast<size_t>(id_stop),
//                                                   static_cast<double>(routing_settings.bus_wait_time)});

//         } else {
//             id_wait = GetNextId(name_stops_ans_wait_id, name_stop,
//             counter_for_id_stops); id_stop = GetNextId(name_stops_and_id,
//             name_stop, counter_for_id_stops); id_edge =
//                 graph.AddEdge(graph::Edge<double>{static_cast<size_t>(id_wait),
//                 static_cast<size_t>(id_stop),
//                                                   static_cast<double>(routing_settings.bus_wait_time)});
//         }
//         previous_name_stop = name_stop;
//         previous_id_stop = id_stop;
//     }
// }

// int ReadRequests::GetNextId(std::map<std::string, int>& name_stops_and_id,
// const std::string& name_stop,
//                             int& counter_for_id_stops) {
//     int id = 0;
//     if (!name_stops_and_id.count(name_stop)) {
//         id = ++counter_for_id_stops;
//         name_stops_and_id[name_stop] = id;
//     } else {
//         id = name_stops_and_id.at(name_stop);
//     }
//     return id;
// }

// double ReadRequests::GetDistance(const std::string& from, const std::string&
// to) {
//     double distance = catalogue_.GetDistanceNextStop(from, to);
//     if (distance == -1) {
//         distance = 0;
//     } else {
//         distance /= routing_settings.bus_velocity;
//     }
//     return distance;
// }