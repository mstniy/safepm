cmake_minimum_required (VERSION 3.1)
set(CMAKE_BUILD_TYPE Debug)
set (CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address")

include(ExternalProject)

ExternalProject_Add(pmdk
  SOURCE_DIR "../../../../pmdk"
  BUILD_IN_SOURCE 1
  CONFIGURE_COMMAND ""
  BUILD_COMMAND $(MAKE)
  INSTALL_COMMAND $(MAKE) install "prefix=${CMAKE_BINARY_DIR}/pmdk/install"
)

include_directories("../../../pmdk/src/include")
link_directories("${CMAKE_BINARY_DIR}/pmdk/install/lib")

project (SPMO CXX)
	file (GLOB SOURCES "../../src/*.cpp")
	add_library (SPMO STATIC ${SOURCES})
	target_include_directories(SPMO PUBLIC "../../include")
	target_link_libraries(SPMO pmemobj)
	add_dependencies(SPMO pmdk)

project (example1 CXX)
	add_executable(example1 example1.cpp)
	add_dependencies(example1 SPMO)
	target_link_libraries(example1 SPMO)
