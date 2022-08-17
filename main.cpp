#include <iostream>
#include <stdexcept>
#include <iomanip>

#include "transport_catalogue.h"
#include "json_reader.h"

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
    Query q(cin);
    SolveQuery(cat, q, cout);
}