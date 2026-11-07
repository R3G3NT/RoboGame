cmake_minimum_required(VERSION 3.16)
project(RoboGame)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Curses REQUIRED)
find_package(GTest REQUIRED)

# Игровая логика отдельной библиотекой
add_library(robogame_core STATIC
        Robot/Robot.cpp
        Field/Field.cpp
        Field/Cell.cpp
        Engine/Visualizer.cpp
        Engine/GameEngine.cpp
        Engine/InputHandler.cpp
        Engine/LevelLoader.cpp
)
target_include_directories(robogame_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(robogame_core PRIVATE ${CURSES_INCLUDE_DIRS})
target_link_libraries(robogame_core PUBLIC ${CURSES_LIBRARIES})

# Сама игра
add_executable(RoboGame main.cpp)
target_link_libraries(RoboGame PRIVATE robogame_core)

# Тесты
enable_testing()
add_executable(robogame_tests
        tests/test_robot.cpp
        tests/test_cell.cpp
        tests/test_field.cpp
        tests/test_gameengine.cpp
        tests/test_statuses.cpp
        tests/test_abilities.cpp
)
target_link_libraries(robogame_tests PRIVATE robogame_core GTest::gtest_main)
include(GoogleTest)
gtest_discover_tests(robogame_tests)