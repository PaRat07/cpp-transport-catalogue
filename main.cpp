#include <iostream>
#include <stdexcept>
#include <iomanip>

#include "transport_catalogue.h"
#include "json_reader.h"
#include "request_handler.h"

using namespace std;
using namespace transport_catalogue;

int main() {
    /*
     * Примерная структура программы:
     *
     * Считать JSON из stdin
     * Построить на его основе JSON базу данных транспортного справочника
     * Выполнить запросы к справочнику, находящиеся в массиве "stat_requests", построив JSON-массив
     * с ответами.
     * Вывести в stdout ответы в виде JSON
     */
    transport_catalogue::TransportCatalogue cat;
    auto query = json::Load(cin);
    MapSettings settings(query.GetRoot().AsMap().at("render_settings").AsMap());
    renderer::MapRenderer map(settings);
    RequestHandler handler(cat, map);
    JsonReader reader(cat, map, handler);
    reader.AddData(query.GetRoot().AsMap().at("base_requests").AsArray());
    json::Print(json::Document(reader.SolveQueries(query.GetRoot().AsMap().at("stat_requests").AsArray())), cout);
}