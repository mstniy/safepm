cmake_minimum_required (VERSION 3.1)
project (pmdk_asan CXX)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_STANDARD 14)

include(ExternalProject)

set(PMDK_INSTALL_DIR "${CMAKE_BINARY_DIR}/pmdk/install")

find_package (Threads)

ExternalProject_Add(pmdk
  SOURCE_DIR "${CMAKE_SOURCE_DIR}/pmdk"
  BUILD_IN_SOURCE 1
  CONFIGURE_COMMAND ""
  BUILD_COMMAND $(MAKE) DEBUG=1
  INSTALL_COMMAND $(MAKE) install DEBUG=1 "prefix=${PMDK_INSTALL_DIR}"
)

add_subdirectory(tests)
