#include "json_reader.h"

Query& Query::operator=(const json::Document &doc) {
    data = doc;
}

std::istream &operator>>(std::istream &in, Query &q) {
    q = json::Load(in).GetRoot();
}

void SolveQuery(transport_catalogue::TransportCatalogue &cat, Query q) {
    std::vector<Query> buses_queries;
    for (const auto &i : q.data.GetRoot().AsMap().at("base_requests").AsArray()) {
        if (i.AsMap().at("type") == "Stop") {
            cat.AddStop()
        }
    }
}