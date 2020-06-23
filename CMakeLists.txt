cmake_minimum_required (VERSION 3.5.1)

project(ChiPP-8 C CXX)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")

SET( CMAKE_EXPORT_COMPILE_COMMANDS ON )
IF ( EXISTS "${CMAKE_CURRENT_BINARY_DIR}/compile_commands.json" )
	EXECUTE_PROCESS( COMMAND ${CMAKE_COMMAND} -E copy_if_different
		${CMAKE_CURRENT_BINARY_DIR}/compile_commands.json
		${CMAKE_CURRENT_SOURCE_DIR}/compile_commands.json
		)
ENDIF()



#[[
find_package(Threads REQUIRED)
set(protobuf_MODULE_COMPATIBLE TRUE)

find_package(Protobuf CONFIG REQUIRED)
message(STATUS "Using protobuf ${protobuf_VERSION}")

set(_PROTOBUF_LIBPROTOBUF protobuf::libprotobuf)
if (CMAKE_CROSSCOMPILING)
	find_program(_PROTOBUF_PROTOC protoc)
else()
	set(_PROTOBUF_PROTOC $<TARGET_FILE:protobuf::protoc>)
endif()

find_package(gRPC CONFIG REQUIRED)
message(STATUS "Using gRPC ${gRPC_VERSION}")

set(_GRPC_GRPCPP_UNSECURE gRPC::grpc++_unsecure)
if (CMAKE_CROSSCOMPILING)
	find_program(_GRPC_CPP_PLUGIN_EXECUTABLE grpc_cpp_plugin)
else()
	set(_GRPC_CPP_PLUGIN_EXECUTABLE $<TARGET_FILE:gRPC::grpc_cpp_plugin>)
endif()


get_filename_component(ts_proto "protos/test.proto" ABSOLUTE)
get_filename_component(ts_proto_path "${ts_proto}" PATH)

set(ts_proto_srcs "${CMAKE_CURRENT_BINARY_DIR}/test.pb.cc")
set(ts_proto_hdrs "${CMAKE_CURRENT_BINARY_DIR}/test.pb.h")
set(ts_grpc_srcs "${CMAKE_CURRENT_BINARY_DIR}/test.grpc.pb.cc")
set(ts_grpc_hdrs "${CMAKE_CURRENT_BINARY_DIR}/test.grpc.pb.h")


add_custom_command(
	OUTPUT "${ts_proto_srcs}" "${ts_proto_hdrs}" "${ts_grpc_srcs}" "${ts_grpc_hdrs}"
	COMMAND ${_PROTOBUF_PROTOC}
	ARGS --grpc_out "${CMAKE_CURRENT_BINARY_DIR}"
		--cpp_out "${CMAKE_CURRENT_BINARY_DIR}"
		-I "${ts_proto_path}"
		--plugin=protoc-gen-grpc="${_GRPC_CPP_PLUGIN_EXECUTABLE}"
		"${ts_proto}"
	DEPENDS "${ts_proto}")

]]

set(chipp-8_srcs "${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp")

message(STATUS "CUrrent Src directory:  ${CMAKE_CURRENT_SOURCE_DIR}")

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIR})
message(STATUS "SDL2 FOUND: ${SDL2_FOUND}")


foreach(_target
		chipp-8)
	add_executable(${_target} 
		${chipp-8_srcs})
	target_link_libraries(${_target}
		${SDL2_LIBRARY})
endforeach()
