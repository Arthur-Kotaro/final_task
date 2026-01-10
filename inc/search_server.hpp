#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <future>
#include <map>
#include <utility>
#include <cstddef>
#include <algorithm>
//#include <thread>

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
    std::vector<std::string> docs;                                                   // Каждый объект типа string это список слов из одного файла. Индекс соответствует... например docs[0] = “milk sugar salt”;
    std::unique_ptr<std::map<std::string, std::vector<entry>>> freq_dictionary;    // “milk”, {0, 1} “слово из запроса”, {индекс файла из docs, количество вхождений};
//    std::map<std::string, std::vector<entry>> *freq_dictionary{};
//    ConverterJSON * converter_json_ptr;

    static std::unique_ptr<std::map<std::string, std::vector<entry>>> separate_indexing(int _doc_id, const std::string & txt_file_content);
    static void merge_auxiliary_maps(std::vector<std::unique_ptr<std::map<std::string, std::vector<entry>>>> &auxiliary_maps, unsigned int hardware_threads);
    static void merge_two_separated_maps(std::vector<std::unique_ptr<std::map<std::string, std::vector<entry>>>> &sep_index_vec, size_t dst, size_t src);
    static void merge_two_sorted_index_vec(std::vector<entry> &dst, std::vector<entry> &src);

public:
    explicit inverted_index(ConverterJSON * _converter_json);
    ~inverted_index();
    void update_document_base(std::vector<std::string> input_docs);
    static std::vector<entry> get_word_count(const std::string &word);
};


class search_server
{
private:
    inverted_index & _index;

public:
    explicit search_server(inverted_index& idx): _index(idx) {}
    std::vector<std::vector<relative_index>> search(const std::vector<std::string>& queries_input);
};