project(commands VERSION 1.0)

# set the library source files
set(COMMANDS_SOURCES commands.c commands.h)
# turn our Event into a library
add_library(${PROJECT_NAME} ${COMMANDS_SOURCES})

option(COMMANDS_DEBUG "Set the commands debug statements on" OFF)
if(COMMANDS_DEBUG)
    add_compile_definitions(COMMANDS_DEBUG)
endif(COMMANDS_DEBUG)

target_include_directories(${PROJECT_NAME} PUBLIC ${ROAD_RUNNER_ROOT}/src ${ROAD_RUNNER_ROOT}/src/modules/commands)