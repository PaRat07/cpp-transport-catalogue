#pragma once


#include <iterator>
#include <string_view>
#include <unordered_set>
#include <optional>

#include "transport_catalogue.h"
#include "svg.h"
#include "domain.h"
#include "map_renderer.h"


// Класс RequestHandler играет роль Фасада, упрощающего взаимодействие JSON reader-а
// с другими подсистемами приложения.
// См. паттерн проектирования Фасад: https://ru.wikipedia.org/wiki/Фасад_(шаблон_проектирования)
class RequestHandler {
public:
    // MapRenderer понадобится в следующей части итогового проекта
    RequestHandler(const transport_catalogue::TransportCatalogue &c,
                   const renderer::MapRenderer &m);

    // Возвращает информацию о маршруте (запрос Bus)
    std::optional<BusStat> GetBusStat(const std::string_view& bus_name) const;

    // Возвращает маршруты, проходящие через
    std::optional<StopsStat> GetStopStat(const std::string_view &stop_name) const;

    std::string RenderMap() const;
private:
    const transport_catalogue::TransportCatalogue& cat_;
    const renderer::MapRenderer &map_;
};
