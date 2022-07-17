# min version of CMAKE for this to work properly
cmake_minimum_required(VERSION 3.12)

# set the project name and version
project(RoadRunner VERSION 1.0)

# set the C_STANDARD to compile too, here we use 90 for C89/90
set(CMAKE_C_STANDARD 90)

# display all output from GCC
add_compile_options(-Wall)

# set the root directory
set(ROAD_RUNNER_ROOT ${PROJECT_SOURCE_DIR})

# tell CMAKE where to put the binary after successful compilation
set(CMAKE_BINARY_DIR ${ROAD_RUNNER_ROOT}/bin)
set (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

# set the source files for the executable
set(ROAD_RUNNER_SOURCES ${ROAD_RUNNER_ROOT}/src/roadrunner.c ${ROAD_RUNNER_ROOT}/src/roadrunner.h)

option(TEST "Set test functions on" OFF)

if(NOT TEST)
    message(STATUS "Normal build, adding RoadRunner executable")
    # add the executable
    add_executable(${PROJECT_NAME} ${ROAD_RUNNER_SOURCES})
endif()

option(RR_DEBUG_ALL "Set all debug statements on" OFF)

if(RR_DEBUG_ALL)
    add_compile_definitions(RR_DEBUG_ALL)
endif(RR_DEBUG_ALL)

option(RR_DEBUG "Set the roadrunner.c debug statements on" OFF)

if(RR_DEBUG)
    add_compile_definitions(RR_DEBUG)
endif(RR_DEBUG)

if(TEST)
    add_compile_definitions(TEST)
endif(TEST)

# creat a variable to store all our lib directories
set(ROAD_RUNNER_LIB_DIRS ${ROAD_RUNNER_ROOT}/src/modules/helloworld ${ROAD_RUNNER_ROOT}/src/modules/commands 
    ${ROAD_RUNNER_ROOT}/src/modules/core ${ROAD_RUNNER_ROOT}/src/modules/files ${ROAD_RUNNER_ROOT}/src/modules/sys
    ${ROAD_RUNNER_ROOT}/src/modules/proxy)
# create a variable to store all our lib names
set(ROAD_RUNNER_LIB_NAMES helloworld commands core files sys proxy)

# tell CMAKE to add all of our library subdirectories
foreach(LIB IN LISTS ROAD_RUNNER_LIB_DIRS)
    message(STATUS "adding ${LIB} directory")
    add_subdirectory(${LIB})
endforeach()

if(TEST)
    message(STATUS "Test build, including test and integration sub directories")

    # add our test subdirectory
    add_subdirectory(${ROAD_RUNNER_ROOT}/test)

    # add our integration subdirectory
    add_subdirectory(${ROAD_RUNNER_ROOT}/integration)
endif()

if(NOT TEST)
    message(STATUS "Normal build, adding include, link, and target libraries")

    # tell CMAKE what directories to search for header files in, this allows us to use #include <example.h>
    # instead of #include "example.h"
    target_include_directories(${PROJECT_NAME} PUBLIC ${ROAD_RUNNER_LIB_DIRS} PUBLIC ${ROAD_RUNNER_ROOT}/src)

    target_link_libraries(${PROJECT_NAME} ${ROAD_RUNNER_LIB_NAMES})
endif()
