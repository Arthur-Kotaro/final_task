#pragma once

#include <cstddef>
#include <iostream>
#include <string>
#include <vector>
#include <map>
//#include <exception>
#include <fstream>

#include "converter_json.hpp"

struct entry
{
    std::size_t doc_id, count;
};

struct relative_index
{
    size_t doc_id;
    float rank;
};


class inverted_index
{
private:
    std::vector<std::string> docs;                  // каждый объект типа string это список слов из одного файла. Индекс соответствует... например docs[0] = “milk sugar salt”;
    std::map<std::string, std::vector<entry>> freq_dictionary; // “milk”, {0, 1} “слово из запроса”, {индекс файла из docs, количество вхождений};


public:
    inverted_index();
    ~inverted_index();
    void update_document_base(std::vector<std::string> input_docs);
    std::vector<entry> get_word_count(const std::string &word);
};


class search_server
{
private:
    inverted_index _index;

public:
    search_server(inverted_index &idx);
    std::vector<std::vector<relative_index>> search(const std::vector<std::string>& queries_input);


};