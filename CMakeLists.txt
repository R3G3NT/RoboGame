cmake_minimum_required(VERSION 4.3)
project(RoboGame)

set(CMAKE_CXX_STANDARD 20)

find_package(Curses REQUIRED)

add_executable(RoboGame main.cpp
        Robot/Robot.h
        Robot/Robot.cpp)

target_include_directories(RoboGame PRIVATE ${CURSES_INCLUDE_DIRS})
target_link_libraries(RoboGame PRIVATE ${CURSES_LIBRARIES})