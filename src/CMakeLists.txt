cmake_minimum_required(VERSION 3.0)
project(sharpPI)

SET(VERSION "1.0")
string(TIMESTAMP DATE "%Y-%m-%dT%H:%M:%SZ")

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/../cmake")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wno-deprecated-declarations")

find_package(Eigen3 REQUIRED)

IF( NOT ${EIGEN3_FOUND} )
    message("EIGEN3 not found!")
ENDIF()

include_directories(${EIGEN3_INCLUDE_DIR})

find_package( Boost COMPONENTS program_options regex filesystem system REQUIRED )
INCLUDE_DIRECTORIES( ${Boost_INCLUDE_DIR} )

add_subdirectory(minisat)
add_subdirectory(glucose)

include(GetGitRevisionDescription)
git_describe(GIT_VERSION --tags --dirty=-d)
get_git_head_revision(GIT_REFSPEC GIT_SHA1)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/../cmake/version.h.in
                ${CMAKE_CURRENT_BINARY_DIR}/version.h)
set(version_file "${CMAKE_CURRENT_BINARY_DIR}/version.h")

set(SOURCE_FILES
  main.cpp
  util.cpp
  sharpsat.cpp sat.cpp
  cbmcparser.cpp
  dimacsparser.cpp
  CommandLineArguments.cpp PICounter.cpp entropy.cpp)

#-- --------------------------------------------------------------------------------------------------------------------
add_executable(sharpPI ${SOURCE_FILES})
target_include_directories(sharpPI PUBLIC "minisat" ${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries(sharpPI  minisat-lib-static  ${Boost_LIBRARIES})

#-- --------------------------------------------------------------------------------------------------------------------
add_executable(sharpPIg ${SOURCE_FILES})
target_include_directories(sharpPIg  PUBLIC "glucose" ${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries(sharpPIg  glucose  ${Boost_LIBRARIES})
target_compile_definitions(sharpPIg PUBLIC -DGLUCOSE)
