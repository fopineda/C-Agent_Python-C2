project(proxy VERSION 1.0)
# set the library source files
set(PROXY_SOURCES proxy.c proxy.h)
# turn our proxy into a library
add_library(${PROJECT_NAME} ${PROXY_SOURCES})

option(PROXY_DEBUG "Set the proxy debug statements on" OFF)
if(PROXY_DEBUG)
    add_compile_definitions(PROXY_DEBUG)
endif(PROXY_DEBUG)

target_include_directories(${PROJECT_NAME} PUBLIC ${ROAD_RUNNER_ROOT}/src/modules/proxy 
                            ${ROAD_RUNNER_ROOT}/src/modules/commands ${ROAD_RUNNER_ROOT}/src)
target_link_libraries(${PROJECT_NAME} commands)

