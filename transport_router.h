#include "router.h"

class TransportRouter : public graph::Router<double> {
public:
    TransportRouter() = default;
    void Initialize(const graph::DirectedWeightedGraph<double> &graph) {
        graph_ = &graph;
        routes_internal_data_ = { graph.GetVertexCount(), std::vector<std::optional<RouteInternalData>>(graph.GetVertexCount()) };
        InitializeRoutesInternalData(graph);

        const size_t vertex_count = graph.GetVertexCount();
        for (graph::VertexId vertex_through = 0; vertex_through < vertex_count; ++vertex_through) {
            RelaxRoutesInternalDataThroughVertex(vertex_count, vertex_through);
        }
    }
};