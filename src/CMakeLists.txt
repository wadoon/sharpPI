cmake_minimum_required(VERSION 3.3)
project(sharpPI)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wno-deprecated-declarations")

find_package(Eigen3 REQUIRED)

IF( NOT ${EIGEN3_FOUND} )
    message("EIGEN3 not found!")
ENDIF()

include_directories(${EIGEN3_INCLUDE_DIR})

find_package( Boost COMPONENTS program_options regex REQUIRED )
INCLUDE_DIRECTORIES( ${Boost_INCLUDE_DIR} )

add_subdirectory(cbmc)
add_subdirectory(minisat)
add_subdirectory(glucose)
add_subdirectory(dsharp)

set(SOURCE_FILES main.cpp util.cpp termbox.cpp CommandLineArguments.cpp PICounter.cpp PICounter.h entropy.cpp entropy.h)

#-- --------------------------------------------------------------------------------------------------------------------
add_executable(sharpPI ${SOURCE_FILES} sharpsat.cpp sharpsat.h)
target_include_directories(sharpPI PUBLIC "minisat" "cbmc" "dsharp" "dsharp/shared")
target_link_libraries(sharpPI cbmc minisat-lib-static dsharp ${Boost_LIBRARIES})

#-- --------------------------------------------------------------------------------------------------------------------
add_executable(sharpPIg ${SOURCE_FILES})
target_include_directories(sharpPIg  PUBLIC "glucose" "cbmc" "dsharp" "dsharp/shared")
target_link_libraries(sharpPIg cbmc glucose dsharp ${Boost_LIBRARIES})
target_compile_definitions(sharpPIg PUBLIC -DGLUCOSE)
