#include <transport_catalogue.pb.h>
#include <fstream>
#include <filesystem>
#include <sstream>

#include "transport_catalogue.h"
#include "map_renderer.h"

using namespace std;

void Serialize(const transport_catalogue::TransportCatalogue &cat, const MapSettings &setting, const filesystem::path &p);

void Deserialize(const filesystem::path &p, transport_catalogue::TransportCatalogue &cat, renderer::MapRenderer &map);
