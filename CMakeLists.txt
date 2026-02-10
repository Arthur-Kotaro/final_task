cmake_minimum_required(VERSION 3.18)
project(search_engine LANGUAGES CXX)


#option(BUILD_TESTING "Build tests" OFF)
#option(BUILD_TESTING "Build tests" ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(JSON_BuildTest OFF CACHE INTERNAL "")
set(gtest_disable_pthreadson)
#set(gtest_force_shared_crt ON CACHE BOOL""FORCE)

add_library(search_lib
        src/search_server.cpp
        src/converter_json.cpp
        src/sapplication.cpp
)

target_include_directories(search_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/inc)
target_link_libraries(search_lib PUBLIC
        nlohmann_json::nlohmann_json
        pthread
)

#set(SOURCES src/main.cpp src/converter_json.cpp src/search_server.cpp src/sapplication.cpp)
#set(HEADERS inc/converter_json.hpp inc/search_server.hpp inc/sapplication.hpp)
#add_executable(search_engine ${SOURCES} ${HEADERS})


add_executable(search_engine src/main.cpp)
target_link_libraries(search_engine PRIVATE
        search_lib
#        gtest_main
)

include(FetchContent)
FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.12.0/json.tar.xz)
FetchContent_MakeAvailable(json)


#if (BUILD_TESTING)
#    enable_testing()
#    FetchContent_Declare(googletest URL https://github.com/google/googletest/archive/refs/heads/main.zip )
#    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
#    FetchContent_MakeAvailable(googletest)
#
#    add_executable(test_search_server tests/test_server.cpp)
#    target_link_libraries(test_search_server PRIVATE search_lib gtest_main)
#    #include_directories(${gtest SOURCE DIR}/include${gtest SOURCE DIR})
#    include(GoogleTest)
#    gtest_discover_tests(search_engine)
#endif()
