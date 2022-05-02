project(BlaCalculate C CXX)
cmake_minimum_required(VERSION 3.3.2)

include(cmake/App.cmake)

set(SOURCES "src/MyApp.h"
            "src/MyApp.cpp"
            "src/main.cpp")

add_app("${SOURCES}")

execute_process (
    COMMAND bash -c "rm -rf ${CMAKE_CURRENT_LIST_DIR}/cmake-build-debug/${PROJECT_NAME}.app"
)

option(RECURSIVE_GENERATE "Recursive call to cmake" OFF)

#if(NOT RECURSIVE_GENERATE)
#    message(STATUS "Recursive generate started")
#    execute_process(COMMAND ${CMAKE_COMMAND}
#            -G "${CMAKE_GENERATOR}"
#            -T "${CMAKE_GENERATOR_TOOLSET}"
#            -A "${CMAKE_GENERATOR_PLATFORM}"
#            -DRECURSIVE_GENERATE:BOOL=ON
#            ${CMAKE_SOURCE_DIR})
#    message(STATUS "Recursive generate done")
#
#    # your post-generate steps here
#    execute_process (
#        COMMAND bash -c "cp ${CMAKE_CURRENT_LIST_DIR}/Icon? ${CMAKE_CURRENT_LIST_DIR}/cmake-build-debug/${PROJECT_NAME}.app/"
#        COMMAND bash -c "chmod ugo+rwx ${CMAKE_CURRENT_LIST_DIR}/cmake-build-debug/${PROJECT_NAME}.app/"
#        COMMAND bash -c "${CMAKE_CURRENT_LIST_DIR}/cmake-build-debug/${PROJECT_NAME}.app/Contents/MacOS/${PROJECT_NAME}"
#    )
#    # exit without doing anything else, since it already happened
#    return()
#endif()