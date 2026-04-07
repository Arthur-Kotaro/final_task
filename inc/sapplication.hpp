#pragma once

#include <fstream>

#include "converter_json.hpp"
#include "search_server.hpp"

#define DEBUG

class SApplication
{
    ConverterJSON* ConverterJSON_ptr;
    InvertedIndex* InvertedIndex_ptr;
    SearchServer* SearchServer_ptr;
public:
    SApplication();
    ~SApplication();
};
