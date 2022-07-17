project(Test VERSION 1.0)

# set the source files (YOU MUST ADD YOUR .c AND .h FILES HERE!)
set(TEST_SOURCES tests.c tests.h commandstests/commandstests.c commandstests/commandstests.h coretests/coretests.c 
        coretests/coretests.h filestests/filestests.c filestests/filestests.h systests/systests.c systests/systests.h)

# display all output from GCC
add_compile_options(-Wall)

# tell CMAKE where to put the binary after successful compilation
set(CMAKE_TEST_BINARY_DIR ${ROAD_RUNNER_ROOT}/test/bin)
set (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_TEST_BINARY_DIR})
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_TEST_BINARY_DIR})

# add the executable
add_executable(${PROJECT_NAME} ${TEST_SOURCES})

# target_include_directories(${PROJECT_NAME} PUBLIC /workspace/libs/ PUBLIC /workspace/test/)
target_include_directories(${PROJECT_NAME} PUBLIC ${ROAD_RUNNER_LIB_DIRS} PUBLIC ${ROAD_RUNNER_ROOT}/test)

target_link_libraries(${PROJECT_NAME} ${ROAD_RUNNER_LIB_NAMES})
