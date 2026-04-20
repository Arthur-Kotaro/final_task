cmake_minimum_required(VERSION 3.18)

add_executable(search_engine src/main.cpp ${SOURCES})
target_include_directories(search_engine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(search_engine PRIVATE pthread nlohmann_json::nlohmann_json)

include(FetchContent)
FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.12.0/json.tar.xz)
FetchContent_MakeAvailable(json)