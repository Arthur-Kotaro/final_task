cmake_minimum_required(VERSION 3.18)
project(search_engine LANGUAGES CXX)

option(BUILD_TESTING "Build tests" ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(JSON_BuildTest OFF CACHE INTERNAL "")
set(gtest_disable_pthreadson)

set(CMAKE_CXX_FLAGS_RELEASE "-03 -DNDEBUG")
set(CMAKE_CXX_FLAGS_DEBUG   "-03 -DDEBUG" )


add_subdirectory(src)
add_subdirectory(tests)
