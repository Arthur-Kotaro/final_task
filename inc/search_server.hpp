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

struct Entry
{
    size_t doc_id;
    size_t count;
};

struct AbsoluteIndex
{
    size_t doc_id;
    size_t absolute_relevance;
};

struct SearchTerm
{
    std::string term;
    size_t count;
};


class InvertedIndex
{
private:
    std::vector<std::string> docs;
    std::unique_ptr<std::map<std::string, std::vector<Entry>>> freq_dictionary;

    static std::unique_ptr<std::map<std::string, std::vector<Entry>>> separate_indexing(int _doc_id, const std::string & txt_file_content);
    static void merge_auxiliary_maps(std::vector<std::unique_ptr<std::map<std::string, std::vector<Entry>>>> &auxiliary_maps, unsigned int hardware_threads);
    static void merge_two_separated_maps(std::vector<std::unique_ptr<std::map<std::string, std::vector<Entry>>>> &sep_index_vec, size_t dst, size_t src);
    static void merge_two_sorted_index_vec(std::vector<Entry> &dst, std::vector<Entry> &src);

public:
    explicit InvertedIndex(ConverterJSON * _converter_json);
    ~InvertedIndex();
    void UpdateDocumentBase(std::vector<std::string> input_docs);
    [[nodiscard]] std::vector<Entry> GetWordCount(const std::string &word) const;
};


class SearchServer
{
private:
	InvertedIndex* _index;
    // std::set<std::string> ParseRequest(const std::string & request);
    [[nodiscard]] std::vector<std::list<SearchTerm>> ParseRequest(const std::vector<std::string>& queries_input) const;
    static size_t FindMaxAbsoluteRelevance(const std::list<AbsoluteIndex>& doc_list);

public:
	explicit SearchServer(InvertedIndex* idx): _index(idx) {}
	[[nodiscard]] std::vector<std::vector<RelativeIndex>> Search(const std::vector<std::string>& queries_input) const;
};
