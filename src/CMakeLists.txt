cmake_minimum_required(VERSION 3.18)


set(SOURCES
    search_server.cpp
    converter_json.cpp
    sapplication.cpp
)

add_executable(search_engine main.cpp ${SOURCES})
target_include_directories(search_engine PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(search_engine PRIVATE pthread nlohmann_json::nlohmann_json)

include(FetchContent)
FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.12.0/json.tar.xz
    DOWNLOAD_EXTRACT_TIMESTAMP
)
FetchContent_MakeAvailable(json)
