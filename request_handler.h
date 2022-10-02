#pragma once


#include <iterator>
#include <string_view>
#include <unordered_set>
#include <optional>
#include <sstream>

#include "transport_catalogue.h"
#include "svg.h"
#include "domain.h"
#include "map_renderer.h"
#include "json.h"

// Класс RequestHandler играет роль Фасада, упрощающего взаимодействие JSON reader-а
// с другими подсистемами приложения.
// См. паттерн проектирования Фасад: https://ru.wikipedia.org/wiki/Фасад_(шаблон_проектирования)
class RequestHandler {
public:
    // MapRenderer понадобится в следующей части итогового проекта
    RequestHandler(transport_catalogue::TransportCatalogue &c,
                   const renderer::MapRenderer &m);

    // Возвращает информацию о маршруте (запрос Bus)
    std::optional<BusStat> GetBusStat(const std::string_view& bus_name) const;

    // Возвращает маршруты, проходящие через
    std::optional<StopsStat> GetStopStat(const std::string_view &stop_name) const;

    std::string RenderMap() const;

    // Возвращает ответы на запросы из потока в виде JSON
    json::Node SolveQueries(const json::Array &data);

    std::optional<RouteStats> GetRoute(std::string_view from, std::string_view to);
private:
    transport_catalogue::TransportCatalogue& cat_;
    const renderer::MapRenderer &map_;
};
