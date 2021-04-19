cmake_minimum_required (VERSION 3.1)
project (pmdk_asan CXX)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_STANDARD 14)

include(ExternalProject)

set(PMDK_INSTALL_DIR "${CMAKE_BINARY_DIR}/pmdk/install")

ExternalProject_Add(pmdk
  SOURCE_DIR "${CMAKE_SOURCE_DIR}/pmdk"
  BUILD_IN_SOURCE 1
  CONFIGURE_COMMAND ""
  BUILD_COMMAND $(MAKE) DEBUG=1
  INSTALL_COMMAND $(MAKE) install DEBUG=1 "prefix=${PMDK_INSTALL_DIR}"
)

file (GLOB SOURCES "src/*.cpp")
add_library (spmo STATIC ${SOURCES})
add_dependencies(spmo pmdk)
target_include_directories(spmo PUBLIC include)
target_link_libraries(spmo pmemobj)
target_include_directories(spmo PUBLIC "${PMDK_INSTALL_DIR}/include")
target_link_directories(spmo PUBLIC "${PMDK_INSTALL_DIR}/lib/pmdk_debug")

add_subdirectory(examples)
