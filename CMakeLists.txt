cmake_minimum_required(VERSION 3.11)

project(p2p_messenger C CXX)

set(CMAKE_CXX_STANDARD 17)

find_package(Protobuf REQUIRED)

add_subdirectory(src)

include(FetchContent)
FetchContent_Declare(
    gRPC
    GIT_REPOSITORY https://github.com/grpc/grpc
    GIT_TAG        v1.34.0
)
set(FETCHCONTENT_QUIET OFF)
FetchContent_MakeAvailable(gRPC)

# Proto generation

foreach(proto_name client_to_server client_to_client)
    get_filename_component(msgr_proto "src/protos/${proto_name}.proto" ABSOLUTE)
    get_filename_component(msgr_proto_path "${msgr_proto}" PATH)
    message(STATUS "Using proto file: ${msgr_proto}")

    set(msgr_proto_srcs "${CMAKE_CURRENT_BINARY_DIR}/${proto_name}.pb.cc")
    set(msgr_proto_hdrs "${CMAKE_CURRENT_BINARY_DIR}/${proto_name}.pb.h")
    set(msgr_grpc_srcs "${CMAKE_CURRENT_BINARY_DIR}/${proto_name}.grpc.pb.cc")
    set(msgr_grpc_hdrs "${CMAKE_CURRENT_BINARY_DIR}/${proto_name}.grpc.pb.h")
    set(_PROTOBUF_PROTOC $<TARGET_FILE:protoc>)
    set(_GRPC_CPP_PLUGIN $<TARGET_FILE:grpc_cpp_plugin>)

    add_custom_command(
            OUTPUT "${msgr_proto_srcs}" "${msgr_proto_hdrs}" "${msgr_grpc_srcs}" "${msgr_grpc_hdrs}"
            COMMAND "${_PROTOBUF_PROTOC}"
            ARGS --grpc_out "${CMAKE_CURRENT_BINARY_DIR}"
                --cpp_out "${CMAKE_CURRENT_BINARY_DIR}"
                -I "${msgr_proto_path}"
                --plugin=protoc-gen-grpc="${_GRPC_CPP_PLUGIN}"
                "${msgr_proto}"
            DEPENDS "${msgr_proto}")

    add_library(${proto_name}_proto_lib
        "${msgr_grpc_srcs}"
        "${msgr_grpc_hdrs}"
        "${msgr_proto_srcs}"
        "${msgr_proto_hdrs}")
    target_link_libraries(${proto_name}_proto_lib grpc++_reflection grpc++ libprotobuf)
endforeach()

# Executables

include_directories("${CMAKE_CURRENT_BINARY_DIR}")

# Test Client

add_executable(test_client src/client/test_client.cpp)
target_link_libraries(test_client PRIVATE
    client_to_server_proto_lib
    client_to_client_proto_lib
    grpc++_reflection
    grpc++
    libprotobuf)

# Test Server

add_executable(test_server src/server/test_server.cpp)
target_link_libraries(test_server PRIVATE
    client_to_server_proto_lib
    grpc++_reflection
    grpc++
    libprotobuf)
