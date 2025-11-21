#cmake_minimum_required(VERSION 3.30)
#project(0_Final)
#
#set(CMAKE_CXX_STANDARD 20)
#
#add_executable(0_Final src/main.cpp inc/main.hpp)
#target_include_directories(0_Final PUBLIC inc)


cmake_minimum_required(VERSION 3.30)
project(search_engine)

set(CMAKE_CXX_STANDARD 20)

add_executable(search_engine src/main.cpp inc/main.hpp)
target_include_directories(search_engine PUBLIC inc)