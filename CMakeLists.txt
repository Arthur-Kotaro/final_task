cmake_minimum_required(VERSION 3.30)
project(search_engine)

set(CMAKE_CXX_STANDARD 20)

set(JSON_BuildTest OFF CACHE INTERNAL "")
set(gtest_disable_pthreadson)
#set(gtest_force_shared_crt ON CACHE BOOL""FORCE)



include(FetchContent)
FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.12.0/json.tar.xz)
FetchContent_Declare(googletestURL https://github.com/google/googletest/archive/refs/heads/main.zip)

FetchContent_MakeAvailable(json)
#FetchContent_MakeAvailable(googletest)

#include_directories(${gtest SOURCE DIR}/include${gtest SOURCE DIR})
#enable_testing()


set(SOURCES src/main.cpp src/converter_json.cpp inc/converter_json.hpp)

add_executable(search_engine ${SOURCES})
target_include_directories(search_engine PUBLIC inc)
target_link_libraries(search_engine PRIVATE nlohmann_json::nlohmann_json
    #    gtest_main
)

#include(GoogleTest)
#gtest_discover_tests(search_engine)
