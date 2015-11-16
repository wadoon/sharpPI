cmake_minimum_required(VERSION 3.3)
set(SOURCE_FILES cbmcparser.cpp dimacsparser.cpp dimacsparser.h)
add_library(cbmc STATIC ${SOURCE_FILES})
