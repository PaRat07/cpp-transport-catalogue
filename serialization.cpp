#include "serialization.h"

transport_catalogue_serialize::Stop SerializeStop(const Stop &stop) {
    transport_catalogue_serialize::Stop to_serialize;
    *to_serialize.mutable_name() = stop.name;
    to_serialize.mutable_coords()->set_lng(stop.coordinates.lng);
    to_serialize.mutable_coords()->set_lat(stop.coordinates.lat);
    return to_serialize;
}

transport_catalogue_serialize::Bus SerializeBus(const Bus &bus) {
    transport_catalogue_serialize::Bus to_serialize;
    to_serialize.set_name(bus.name);
    for (const Stop *stop : bus.stops) {
        to_serialize.mutable_stops()->Add(SerializeStop(*stop));
    }
    to_serialize.set_is_roundtrip(bus.is_roundtrip);
    return to_serialize;
}

void Serialize(const transport_catalogue::TransportCatalogue &cat, const filesystem::path &p) {
    ofstream out(p, ios::binary);
    transport_catalogue_serialize::Catalogue to_serialize;
    for (const Bus &bus : cat.GetBuses()) {
        to_serialize.mutable_buses()->Add(SerializeBus(bus));
    }
    to_serialize.SerializeToOstream(&out);
}
