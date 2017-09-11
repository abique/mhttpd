cmake_minimum_required(VERSION 2.8)

project(mhttpd C CXX)

enable_testing()

find_package(GnuTLS REQUIRED)

file(STRINGS VERSION MHTTPD_VERSION)

message("mhttpd version: ${MHTTPD_VERSION}")

add_definitions(
  -std=c++14
  -D_GNU_SOURCE -D_LARGEFILE64_SOURCE
  -Wall -Wextra -Werror=return-type
  ${GNUTLS_DEFINITIONS})

set(CMAKE_C_FLAGS "${CMAKE_CXX_FLAGS} -std=c99")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

add_subdirectory(mimosa EXCLUDE_FROM_ALL)
include_directories(mimosa)

include(CheckIncludeFiles)
include(CheckLibraryExists)

add_subdirectory(mhttpd)
