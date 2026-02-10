#pragma once

#include <algorithm>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <future>
#include <list>
#include <map>
// #include <set>
#include <utility>
#include <cstddef>
//#include <thread>

#include "converter_json.hpp"

struct entry
{
    size_t doc_id;
    size_t count;
};

struct absolute_index
{
    size_t doc_id;
    size_t absolute_relevance;
};

struct relative_index
{
    size_t doc_id;
    float rank;
};

struct search_term
{
    std::string term;
    size_t count;
};


class inverted_index
{
private:
    std::vector<std::string> docs;
    std::unique_ptr<std::map<std::string, std::vector<entry>>> freq_dictionary;

    static std::unique_ptr<std::map<std::string, std::vector<entry>>> separate_indexing(int _doc_id, const std::string & txt_file_content);
    static void merge_auxiliary_maps(std::vector<std::unique_ptr<std::map<std::string, std::vector<entry>>>> &auxiliary_maps, unsigned int hardware_threads);
    static void merge_two_separated_maps(std::vector<std::unique_ptr<std::map<std::string, std::vector<entry>>>> &sep_index_vec, size_t dst, size_t src);
    static void merge_two_sorted_index_vec(std::vector<entry> &dst, std::vector<entry> &src);

public:
    explicit inverted_index(ConverterJSON * _converter_json);
    ~inverted_index();
    void update_document_base(std::vector<std::string> input_docs);
    std::vector<entry> get_word_count(const std::string &word) const;
};


class search_server
{
private:
	inverted_index* _index;
    // std::set<std::string> ParseRequest(const std::string & request);
    [[nodiscard]] std::vector<std::list<search_term>> ParseRequest(const std::vector<std::string>& queries_input) const;
    static size_t FindMaxAbsoluteRelevance(const std::list<absolute_index>& doc_list);

public:
	explicit search_server(inverted_index* idx): _index(idx) {}
	std::vector<std::vector<relative_index>> search(const std::vector<std::string>& queries_input) const;
};
