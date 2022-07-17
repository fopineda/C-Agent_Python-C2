project(files VERSION 1.0)

# set the library source files
set(FILES_SOURCES files.c files.h)
# turn into a library
add_library(${PROJECT_NAME} ${FILES_SOURCES})

option(FILES_DEBUG "Set the files debug statements on" OFF)
if(FILES_DEBUG)
    add_compile_definitions(FILES_DEBUG)
endif(FILES_DEBUG)

target_include_directories(${PROJECT_NAME} PUBLIC  ${ROAD_RUNNER_ROOT}/src ${ROAD_RUNNER_ROOT}/src/modules/files 
                            ${ROAD_RUNNER_ROOT}/src/modules/commands)
target_link_libraries(${PROJECT_NAME} commands)