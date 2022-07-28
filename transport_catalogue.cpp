#include "transport_catalogue.h"

using namespace std;

Stop::Stop(Stop &&stop)
    : name(exchange(stop.name, ""s))
    , latitude(exchange(stop.latitude, 0.0))
    , longitude(exchange(stop.longitude, 0.0))
{
}

Stop::Stop(string_view n, double lat, double lon)
    : name(n)
    , latitude(lat)
    , longitude(lon)
{
}

Bus::Bus(Bus &&bus)
    : name(exchange(bus.name, ""s))
    , stops(exchange(bus.stops, {}))
{
}

Bus::Bus(string_view n, std::vector<const Stop *> s)
    : name(n)
    , stops(s)
{
}

void TransportCatalogue::AddBus(std::string_view bus_name, std::vector<std::string_view> stops) {

}