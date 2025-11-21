cmake_minimum_required(VERSION 3.30)
project(search_engine)

set(CMAKE_CXX_STANDARD 20)

set(SOURCES src/main.cpp src/converter_json.cpp inc/converter_json.hpp)

add_executable(search_engine ${SOURCES})
target_include_directories(search_engine PUBLIC inc)
