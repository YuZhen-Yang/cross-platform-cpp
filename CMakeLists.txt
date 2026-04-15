cmake_minimum_required(VERSION 3.29)

project(CrossPlatformCpp
    VERSION 1.0.0
    LANGUAGES CXX
)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_NINJA_FORCE_RESPONSE_FILE ON CACHE BOOL "" FORCE)

add_subdirectory(test/lib_validation)
