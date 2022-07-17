project(helloworld VERSION 1.0)
# set the library source files
set(HELLO_WORLD_SOURCES helloworld.c helloworld.h)
# turn our HelloWorld into a library
add_library(${PROJECT_NAME} ${HELLO_WORLD_SOURCES})

option(HELLOWORLD_DEBUG "Set the helloworld debug statements on" OFF)
if(HELLOWORLD_DEBUG)
    add_compile_definitions(HELLOWORLD_DEBUG)
endif(HELLOWORLD_DEBUG)

target_include_directories(${PROJECT_NAME} PUBLIC ${ROAD_RUNNER_ROOT}/src/modules/helloworld)
