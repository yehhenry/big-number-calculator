project(BlaCalculate C CXX)
cmake_minimum_required(VERSION 3.3.2)

include(cmake/App.cmake)

set(SOURCES "src/MyApp.h"
        "src/MyApp.cpp"
        "src/main.cpp"
        "../big-number-calculator/Blamath.h"
        "../big-number-calculator/Blamath.cpp"
        "../big-number-calculator/Decimal.h"
        "../big-number-calculator/Decimal.cpp"
        "../big-number-calculator/Integer.h"
        "../big-number-calculator/Integer.cpp")

add_app("${SOURCES}")

execute_process (
        COMMAND bash -c "rm -rf ${CMAKE_CURRENT_LIST_DIR}/cmake-build-debug/${PROJECT_NAME}.app"
)

option(RECURSIVE_GENERATE "Recursive call to cmake" OFF)