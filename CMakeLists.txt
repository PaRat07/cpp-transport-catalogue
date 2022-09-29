cmake_minimum_required(VERSION 3.22)
project(yandex_directory)

set(CMAKE_CXX_STANDARD 17)

add_executable(cpp-transport-catalogue main.cpp geo.h transport_catalogue.cpp transport_catalogue.h domain.cpp domain.h json.cpp json.h json_reader.cpp json_reader.h request_handler.cpp request_handler.h geo.cpp map_renderer.cpp map_renderer.h svg.cpp svg.h json_builder.cpp json_builder.h transport_router.cpp transport_router.h graph.h ranges.h router.h)
