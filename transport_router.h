#include "router.h"

class TransportRouter : public graph::Router<double> {
public:
    TransportRouter() = default;
    void Initialize(const graph::DirectedWeightedGraph<double> &graph);
};