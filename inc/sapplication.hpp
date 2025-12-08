#pragma once

#include <fstream>

#include "converter_json.hpp"
#include "search_server.hpp"

class SApplication
{
    ConverterJSON* ConverterJSON_ptr;
    inverted_index* inverted_index_ptr;
    search_server* search_server_ptr;
public:
    SApplication();
    ~SApplication();
};
