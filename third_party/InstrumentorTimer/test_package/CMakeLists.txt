cmake_minimum_required(VERSION 3.15)
project(test_instrumentor CXX)

find_package(InstrumentorTimer REQUIRED)

add_executable(test_instrumentor main.cpp)
target_link_libraries(test_instrumentor PRIVATE InstrumentorTimer::InstrumentorTimer)
target_compile_features(test_instrumentor PRIVATE cxx_std_11)