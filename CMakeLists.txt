project(BlaCalculate C CXX)
cmake_minimum_required(VERSION 3.3.2)

include(cmake/App.cmake)

set(SOURCES "src/MyApp.h"
        "src/MyApp.cpp"
        "src/main.cpp"
        "src/Blamath.h"
        "src/Blamath.cpp"
        "src/Decimal.h"
        "src/Decimal.cpp"
        "src/Integer.h"
        "src/Integer.cpp")

add_app("${SOURCES}")

execute_process (
        COMMAND bash -c "rm -rf ${CMAKE_CURRENT_LIST_DIR}/cmake-build-debug/${PROJECT_NAME}.app"
)

option(RECURSIVE_GENERATE "Recursive call to cmake" OFF)