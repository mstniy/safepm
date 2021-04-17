cmake_minimum_required (VERSION 3.1)
set (CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address")

project (SPMO CXX)
	file (GLOB SOURCES "../../src/*.cpp")
	add_library (SPMO STATIC ${SOURCES})
	target_include_directories(SPMO PUBLIC "../../include")
	target_link_libraries(SPMO pmemobj)

project (example1 CXX)
	add_executable(example1 example1.cpp)
	add_dependencies(example1 SPMO)
	target_link_libraries(example1 SPMO)
