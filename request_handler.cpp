#include "request_handler.h"


std::optional<BusStat> RequestHandler::GetBusStat(const std::string_view &bus_name) const {
    return cat_.GetDataForBus(bus_name);
}

std::optional<StopsStat> RequestHandler::GetStopStat(const std::string_view &stop_name) const {
    return cat_.GetDataForStop(stop_name);
}

std::string RequestHandler::RenderMap() const {
    std::ostringstream ans;
    map_.Render(ans);
    return ans.str();
}

RequestHandler::RequestHandler(const transport_catalogue::TransportCatalogue &c,
                               const renderer::MapRenderer &m)
        : cat_(c)
        , map_(m)
{

}
