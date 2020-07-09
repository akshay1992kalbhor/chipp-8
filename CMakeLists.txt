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
