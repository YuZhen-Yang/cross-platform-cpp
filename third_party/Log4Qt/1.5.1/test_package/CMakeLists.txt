cmake_minimum_required(VERSION 3.15)
project(Log4QtTest CXX)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Log4Qt REQUIRED)
find_package(Qt5 REQUIRED COMPONENTS Core Concurrent)

add_executable(Log4QtTest test.cpp)
target_link_libraries(Log4QtTest PRIVATE Log4Qt::Log4Qt Qt5::Core Qt5::Concurrent)
