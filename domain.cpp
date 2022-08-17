#include <utility>
#include "domain.h"

using namespace std;

Stop::Stop(Stop &&stop)
        : name(exchange(stop.name, "")), coordinates()
{
}

Stop::Stop(string_view n, double lat, double lng)
        : name(n), coordinates({lat, lng})
{
}

Bus::Bus(Bus &&bus) noexcept
        : name(exchange(bus.name, ""s)), stops(exchange(bus.stops, {}))
{
}

Bus::Bus(string_view n, const std::vector<const Stop *> &s)
        : name(n), stops(s)
{
}