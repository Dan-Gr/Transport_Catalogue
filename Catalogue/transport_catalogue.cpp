#include "transport_catalogue.h"

// --------- блок функций при чтении из JSON --------- начало
void TransportCatalogue::SetDataBusRoute(const DataBusRoute& data_bus_route,
                                         std::deque<std::string>::iterator name_route) {
    data_routes_json_[*name_route] = data_bus_route;
}

void TransportCatalogue::SetDataStop(const DataStop& data_stop, std::deque<std::string>::iterator stop_name) {
    data_stops_buses_json_[*stop_name] = data_stop;
}

std::deque<std::string>::iterator TransportCatalogue::SaveNameStop(const std::string& stop_name) {
    std::deque<std::string>::iterator pos_insert =
        save_names_stops_buses_.insert(save_names_stops_buses_.begin(), stop_name);
    return pos_insert;
}

std::deque<std::string>::iterator TransportCatalogue::SaveNameRoute(const std::string& num_route) {
    std::deque<std::string>::iterator pos_insert =
        save_names_routes.insert(save_names_routes.begin(), num_route);
    return pos_insert;
}

bool TransportCatalogue::CountRoute(std::string_view num_route) {
    return std::count(save_names_routes.begin(), save_names_routes.end(), num_route);
}

bool TransportCatalogue::CountStop(std::string_view name_stops) {
    return std::count(save_names_stops_buses_.begin(), save_names_stops_buses_.end(), name_stops);
}

std::deque<std::string>::iterator TransportCatalogue::GetIteratorNamesStop(std::string_view name_stop) {
    return std::find(save_names_stops_buses_.begin(), save_names_stops_buses_.end(), name_stop);
}

std::deque<std::string>::iterator TransportCatalogue::GetIteratorNamesRoute(std::string_view num_route) {
    return std::find(save_names_routes.begin(), save_names_routes.end(), num_route);
}

double TransportCatalogue::GetCurvature(std::string_view num_route) {
    double curvature = (GetRealDistance(num_route) / 2) / GetGeographicalDistance(num_route);
    if (data_routes_json_.at(num_route).GetTypeRoute() == ROUND) {
        curvature *= 2;
    }
    return curvature;
}

double TransportCatalogue::GetRealDistance(std::string_view num_route) {
    double real_distance = 0;
    if (data_routes_json_.at(num_route).GetTypeRoute() == LINE) {
        real_distance = GetRealDistanceLine(num_route);
    } else {
        real_distance = GetRealDistanceRound(num_route);
    }
    return real_distance;
}

double TransportCatalogue::GetRealDistanceLine(std::string_view num_route) {
    double ans = 0;
    std::list<std::string> route(data_routes_json_.at(num_route).GetRoute());
    std::string_view stop_one(route.front());
    std::string_view stop_two;
    ans += GetRealDistanceLineInOneDirection(route, stop_one, stop_two);
    route.reverse();
    stop_one = route.front();
    ans += GetRealDistanceLineInOneDirection(route, stop_one, stop_two);
    return ans;
}

double TransportCatalogue::GetRealDistanceLineInOneDirection(const std::list<std::string>& route,
                                                             std::string_view& stop_one,
                                                             std::string_view& stop_two) {
    double ans = 0;
    int fl = 0;
    for (const std::string_view stop : route) {
        stop_two = stop;
        if (fl) {
            int dis = GetDistanceNextStop(stop_one, stop_two);
            if (dis == -1) {
                dis = 0;
            }
            ans += dis;
            stop_one = stop_two;
        }
        fl = 1;
    }
    int dis = GetDistanceNextStop(stop_one, stop_two);
    if (dis == -1) {
        dis = 0;
    }
    ans += dis;
    return ans;
}

double TransportCatalogue::GetRealDistanceRound(std::string_view num_route) {
    double ans = 0;
    std::list<std::string> route(data_routes_json_.at(num_route).GetRoute());
    std::string_view stop_one(route.front());
    std::string_view stop_two;
    ans += GetRealDistanceLineInOneDirection(route, stop_one, stop_two);
    return ans;
}

double TransportCatalogue::GetGeographicalDistance(std::string_view num_route) {
    double ans = 0;
    std::list<std::string> route(data_routes_json_.at(num_route).GetRoute());
    std::string stop_one(route.front());
    std::string stop_two;
    int fl = 0;
    for (const auto& stop : route) {
        stop_two = stop;
        if (fl) {
            ans += geo::ComputeDistance(data_stops_buses_json_.at(stop_one).GetCoordinatesStop(),
                                        data_stops_buses_json_.at(stop_two).GetCoordinatesStop());
            stop_one = stop_two;
        }
        fl = 1;
    }
    ans += geo::ComputeDistance(data_stops_buses_json_.at(stop_one).GetCoordinatesStop(),
                                data_stops_buses_json_.at(stop_two).GetCoordinatesStop());
    return ans;
}

int TransportCatalogue::GetStopCount(std::string_view num_route) {
    int ans = 0;
    if (data_routes_json_.count(num_route)) {
        ans = data_routes_json_.at(num_route).GetCoutStop();
    }
    return ans;
}
int TransportCatalogue::GetStopUniqueCount(std::string_view num_route) {
    int ans = 0;
    if (data_routes_json_.count(num_route)) {
        ans = data_routes_json_.at(num_route).GetCountUniqueStop();
    }
    return ans;
}

size_t TransportCatalogue::GetAllStopCount() const { return data_stops_buses_json_.size(); }

int TransportCatalogue::GetCountRoutes() { return data_routes_json_.size(); }

int TransportCatalogue::GetIdStop(std::string_view name_stop) {
    return data_stops_buses_json_.at(name_stop).GetId();
}

std::string TransportCatalogue::GetNameStop(int id) {
    std::string answer = "";
    for (auto& [name_stop, data_stop] : data_stops_buses_json_) {
        if (data_stop.GetId() == id) {
            answer = name_stop;
            break;
        }
    }
    return answer;
}

std::string TransportCatalogue::GetCommonRoute(int id_first_stop, int id_second_stop) {
    std::string answer = "";
    std::string name_id_first_stop = GetNameStop(id_first_stop);
    std::string name_id_second_stop = GetNameStop(id_second_stop);
    for (auto& [name_route, data_route] : data_routes_json_) {
        if (data_route.CheckStopInRoute(name_id_first_stop) &&
            data_route.CheckStopInRoute(name_id_second_stop)) {
            answer = name_route;
            break;
        }
    }
    return answer;
}

std::string TransportCatalogue::GetCommonRoute(std::string id_first_stop, std::string id_second_stop) {
    std::string answer = "";
    for (auto& [name_route, data_route] : data_routes_json_) {
        if (data_route.CheckStopInRoute(id_first_stop) && data_route.CheckStopInRoute(id_second_stop)) {
            answer = name_route;
            break;
        }
    }
    return answer;
}

std::map<std::string, std::vector<geo::Coordinates>> TransportCatalogue::GetCoordinatesAllRoutes() {
    std::map<std::string, std::vector<geo::Coordinates>> res;
    for (auto& [name, data] : data_routes_json_) {
        std::vector<geo::Coordinates> buf;
        buf.clear();
        buf.reserve(data_routes_json_.size());
        std::list<std::string> rout = data.GetRoute();
        for (auto& stop : rout) {
            buf.push_back(data_stops_buses_json_.at(stop).GetCoordinatesStop());
        }
        if (data.GetTypeRoute() == LINE) {
            rout.reverse();
            int fl = 0;
            for (auto& stop : rout) {
                if (fl) {
                    buf.push_back(data_stops_buses_json_.at(stop).GetCoordinatesStop());
                }
                fl = 1;
            }
            fl = 0;
        }
        std::string n(name);
        res.insert(res.begin(), {n, buf});
    }
    return res;
}

std::vector<std::string> TransportCatalogue::GetRoutesInStop(std::string_view name_stop) {
    std::vector<std::string> result;
    std::string search(name_stop);
    for (auto& check : data_routes_json_) {
        if (check.second.CheckStopInRoute(search)) {
            std::deque<std::string>::iterator pos = GetIteratorNamesRoute(check.first);
            result.push_back(*pos);
        }
    }
    return result;
}

std::list<std::string> TransportCatalogue::GetStopsInRoute(std::string_view name_route) {
    return data_routes_json_.at(name_route).GetRoute();
}

double TransportCatalogue::GetDistanceNextStop(std::string_view stop_one, std::string_view stop_two) {
    double ans = 0;
    if (data_stops_buses_json_.at(stop_one).GetDistance(stop_two) != -1) {
        ans = data_stops_buses_json_.at(stop_one).GetDistance(stop_two);
    } else if (data_stops_buses_json_.at(stop_two).GetDistance(stop_one) != -1) {
        ans = data_stops_buses_json_.at(stop_two).GetDistance(stop_one);
    }
    return ans;
}

TYPE_ROUTE TransportCatalogue::GetTypeRoute(std::string_view name_route) {
    return data_routes_json_.at(name_route).GetTypeRoute();
}

std::string TransportCatalogue::GetNameStopsInCoordinates(const geo::Coordinates& coordinate) {
    std::string result;
    for (auto& search : data_stops_buses_json_) {
        if (search.second.GetCoordinatesStop() == coordinate) {
            std::string buf(search.first);
            result = buf;
            break;
        }
    }
    return result;
}

// --------- блок функций при чтении из JSON --------- конец