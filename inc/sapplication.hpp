#pragma once

#include <fstream>

#include "converter_json.hpp"
#include "search_server.hpp"

#define DEBUG

class SApplication
{
    ConverterJSON* ConverterJSONPtr;
    InvertedIndex* InvertedIndexPtr;
    SearchServer* SearchServerPtr;
public:
    SApplication();
    ~SApplication();
};
