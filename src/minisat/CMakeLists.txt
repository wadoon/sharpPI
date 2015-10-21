cmake_minimum_required(VERSION 2.6 FATAL_ERROR)
project(minisat)

#--------------------------------------------------------------------------------------------------
# Library version:

set(MINISAT_SOMAJOR   2)
set(MINISAT_SOMINOR   1)
set(MINISAT_SORELEASE 0)

set(MINISAT_VERSION ${MINISAT_SOMAJOR}.${MINISAT_SOMINOR}.${MINISAT_SORELEASE})
set(MINISAT_SOVERSION ${MINISAT_SOMAJOR})

#--------------------------------------------------------------------------------------------------
# Dependencies:

find_package(ZLIB)
include_directories(${ZLIB_INCLUDE_DIR})
include_directories(${minisat_SOURCE_DIR})

#--------------------------------------------------------------------------------------------------
# Compile flags:

add_definitions(-D__STDC_FORMAT_MACROS -D__STDC_LIMIT_MACROS)

set(CMAKE_CXX_FLAGS "-Wliteral-suffix")

#--------------------------------------------------------------------------------------------------
# Build Targets:

set(MINISAT_LIB_SOURCES
    utils/Options.cc
    utils/System.cc
    core/Solver.cc
    simp/SimpSolver.cc)

add_library(minisat-lib-static STATIC ${MINISAT_LIB_SOURCES})
target_link_libraries(minisat-lib-static ${ZLIB_LIBRARY})

#add_executable(minisat_core minisat/core/Main.cc)
#add_executable(minisat_simp minisat/simp/Main.cc)
#target_link_libraries(minisat_core minisat-lib-static)
#target_link_libraries(minisat_simp minisat-lib-static)

set_target_properties(minisat-lib-static PROPERTIES OUTPUT_NAME "minisat")