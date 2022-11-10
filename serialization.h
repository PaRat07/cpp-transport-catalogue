#include <transport_catalogue.pb.h>
#include <fstream>
#include <filesystem>

#include "transport_catalogue.h"

using namespace std;

void Serialize(const transport_catalogue::TransportCatalogue &cat, const filesystem::path &p);

transport_catalogue::TransportCatalogue Deserialize(const filesystem::path &p);
