cmake_minimum_required(VERSION 3.3)
project(sharpPIC)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wno-deprecated-declarations")

find_package(Eigen3 REQUIRED)

IF( NOT ${EIGEN3_FOUND} )
    message("EIGEN3 not found!")
ENDIF()

include_directories(${EIGEN3_INCLUDE_DIR})

find_package( Boost COMPONENTS program_options REQUIRED )
INCLUDE_DIRECTORIES( ${Boost_INCLUDE_DIR} )

add_subdirectory(cbmc)
add_subdirectory(minisat)

include_directories(minisat cbmc)

set(SOURCE_FILES main.cpp util.cpp termbox.cpp CommandLineArguments.cpp PICounter.cpp PICounter.h entropy.cpp entropy.h)
add_executable(sharpPIC ${SOURCE_FILES})
target_link_libraries(sharpPIC cbmc minisat-lib-static ${Boost_LIBRARIES})
