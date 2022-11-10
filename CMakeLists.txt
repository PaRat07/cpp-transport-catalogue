cmake_minimum_required(VERSION 3.22)
project(cpp-transport-catalogue)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_PREFIX_PATH D:/Pasha/yandex/cpp-transport-catalogue/protobuf)

find_package(Thread REQUIRED)
find_package(Protobuf REQUIRED)

protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS transport_catalogue.proto)

add_executable(make-base make_base.cpp ${PROTO_HDRS} transport_catalogue.h transport_catalogue.cpp json.h json.cpp map_renderer.h map_renderer.cpp svg.h svg.cpp domain.h domain.cpp geo.h geo.cpp transport_router.h transport_router.cpp serialization.h serialization.cpp)

target_include_directories(make-base PUBLIC ${Protobuf_INCLUDE_DIRS})
target_include_directories(make-base PUBLIC ${Protobuf_BINARY_DIR})
target_include_directories(make-base PUBLIC ${CMAKE_CURRENT_BINARY_DIR})

target_link_libraries(make-base "$<IF:$<CONFIG:Debug>,${Protobuf_LIBRARY_DEBUG},${Protobuf_LIBRARY_RELEASE}>" Threads::Threads)

add_executable(process_requests process_requests.cpp ${PROTO_SRC} ${PROTO_HDRS} transport_catalogue.h transport_catalogue.cpp geo.h geo.cpp graph.h domain.h domain.cpp transport_router.h transport_router.cpp json_reader.h json_reader.cpp json.h json.cpp request_handler.h request_handler.cpp json_builder.h json_builder.cpp map_renderer.h map_renderer.cpp svg.h svg.cpp serialization.h serialization.cpp)

target_include_directories(process_requests PUBLIC ${Protobuf_INCLUDE_DIRS})
target_include_directories(process_requests PUBLIC ${CMAKE_CURRENT_BINARY_DIR})
