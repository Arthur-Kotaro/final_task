cmake_minimum_required(VERSION 3.18)

if (BUILD_TESTING)
    enable_testing()

    FetchContent_Declare(googletest URL https://github.com/google/googletest/archive/refs/heads/main.zip 
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE)
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)
    set(S_DIR ${PROJECT_SOURCE_DIR}/src)
    set(SOURCES
        ${S_DIR}/search_server.cpp
        ${S_DIR}/converter_json.cpp
        ${S_DIR}/sapplication.cpp
    ) 
    add_executable(test_search_server test_server.cpp ${SOURCES})
    target_include_directories(test_search_server PUBLIC ${PROJECT_SOURCE_DIR}/include)
    target_link_libraries(test_search_server PRIVATE gtest_main nlohmann_json::nlohmann_json)

    #include_directories(${gtest_SOURCE_DIR}/include${gtest_SOURCE_DIR})
    include(GoogleTest)
    gtest_discover_tests(test_search_server)
endif()
