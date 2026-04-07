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
    size_t docID;
    size_t count;
    bool operator==(const Entry & other) const;
};

struct AbsoluteIndex
{
    size_t docID;
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

    static std::unique_ptr<std::map<std::string, std::vector<Entry>>> SeparateIndexing(int _doc_id, const std::string & txt_file_content);
    static void MergeAuxiliaryMaps(std::vector<std::unique_ptr<std::map<std::string, std::vector<Entry>>>> &auxiliary_maps, unsigned int hardware_threads);
    static void MergeTwoSeparatedMaps(std::vector<std::unique_ptr<std::map<std::string, std::vector<Entry>>>> &sep_index_vec, size_t dst, size_t src);
    static void MergeTwoSortedIndexVec(std::vector<Entry> &dst, std::vector<Entry> &src);

public:
    explicit InvertedIndex(ConverterJSON * _converter_json);
    ~InvertedIndex();
    void UpdateDocumentBase(std::vector<std::string> input_docs);
    std::vector<Entry> GetWordCount(const std::string &word);
};


class SearchServer
{
private:
	InvertedIndex* _index; 
    [[nodiscard]] std::vector<std::list<SearchTerm>> ParseRequest(const std::vector<std::string>& queries_input) const;
    static size_t FindMaxAbsoluteRelevance(const std::list<AbsoluteIndex>& doc_list);

public:
	explicit SearchServer(InvertedIndex* idx): _index(idx) {}
	[[nodiscard]] std::vector<std::vector<RelativeIndex>> Search(const std::vector<std::string>& queries_input) const;
};
