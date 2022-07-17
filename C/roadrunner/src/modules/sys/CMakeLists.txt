project(sys VERSION 1.0)

# set the library source files
set(SYS_SOURCES sys.c sys.h)
# turn into a library
add_library(${PROJECT_NAME} ${SYS_SOURCES})

option(SYS_DEBUG "Set the files debug statements on" OFF)
if(SYS_DEBUG)
    add_compile_definitions(SYS_DEBUG)
endif(SYS_DEBUG)

target_include_directories(${PROJECT_NAME} PUBLIC  ${ROAD_RUNNER_ROOT}/src ${ROAD_RUNNER_ROOT}/src/modules/sys 
                            ${ROAD_RUNNER_ROOT}/src/modules/commands ${ROAD_RUNNER_ROOT}/src/modules/files)
target_link_libraries(${PROJECT_NAME} commands)
target_link_libraries(${PROJECT_NAME} files)
