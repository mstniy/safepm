cmake_minimum_required (VERSION 3.1)
project (pmdk_asan CXX)

#set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_STANDARD 14)

include(ExternalProject)

set(PMDK_INSTALL_DIR "${CMAKE_BINARY_DIR}/pmdk/install")

find_package (Threads)

string(TOLOWER "${CMAKE_BUILD_TYPE}" build_type)
if ("${build_type}" STREQUAL "debug")
  set(DEBUG 1)
  set(PMDK_LIB_DIR "${PMDK_INSTALL_DIR}/lib/pmdk_debug")
  message(STATUS "Debug build")
else()
  set(DEBUG "")
  set(PMDK_LIB_DIR "${PMDK_INSTALL_DIR}/lib")
  message(STATUS "Release build")
endif()

ExternalProject_Add(pmdk
  SOURCE_DIR "${CMAKE_SOURCE_DIR}/pmdk"
  BUILD_IN_SOURCE 1
  BUILD_ALWAYS 1
  CONFIGURE_COMMAND ""
  BUILD_COMMAND $(MAKE) DEBUG=${DEBUG}
  INSTALL_COMMAND $(MAKE) install "prefix=${PMDK_INSTALL_DIR}" DEBUG=${DEBUG}
)

add_library(libpmemobj STATIC IMPORTED)
set_property(TARGET libpmemobj PROPERTY IMPORTED_LOCATION "${PMDK_LIB_DIR}/libpmemobj.a")
add_dependencies(libpmemobj pmdk)

add_library(libpmem STATIC IMPORTED)
set_property(TARGET libpmem PROPERTY IMPORTED_LOCATION "${PMDK_LIB_DIR}/libpmem.a")
add_dependencies(libpmem pmdk)

add_subdirectory(tests)
