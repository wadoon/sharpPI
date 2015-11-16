cmake_minimum_required(VERSION 2.8)
project(dsharp)
file(GLOB_RECURSE lib_srcs RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.cpp)
list(REMOVE_ITEM lib_srcs src_sharpSAT/main.cpp)

message(${lib_srcs})

add_library(dsharp ${lib_srcs})
target_include_directories(dsharp PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/shared)


