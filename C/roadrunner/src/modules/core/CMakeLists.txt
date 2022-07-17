project(core VERSION 1.0)
# set the library source files
set(CORE_SOURCES core.c core.h)
# turn our core into a library
add_library(${PROJECT_NAME} ${CORE_SOURCES})

option(CORE_DEBUG "Set the core debug statements on" OFF)
if(CORE_DEBUG)
    add_compile_definitions(CORE_DEBUG)
endif(CORE_DEBUG)

target_include_directories(${PROJECT_NAME} PUBLIC ${ROAD_RUNNER_ROOT}/src PUBLIC ${ROAD_RUNNER_ROOT}/src/modules/core 
                        PUBLIC ${ROAD_RUNNER_ROOT}/src/modules/commands)
target_link_libraries(${PROJECT_NAME} commands)
