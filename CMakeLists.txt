cmake_minimum_required(VERSION 3.22)
project(yandex_directory)

set(CMAKE_CXX_STANDARD 17)

add_executable(cpp-transport-catalogue main.cpp geo.h input_reader.cpp input_reader.h stat_reader.cpp stat_reader.h transport_catalogue.cpp transport_catalogue.h log_duration.h)
