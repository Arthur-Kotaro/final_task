#pragma once

#include <cstddef>
#include <exception>
#include <string>
#include <vector>
#include <map>

#include "nlohmann/json.hpp"
//#include "gtest/gtest.h"

#define VERSION 0.1


struct entry
{
    std::size_t doc_id, count;
};

struct relative_index
{
    size_t doc_id;
    float rank;
};



class ConverterJSON
{
public:
    std::vector<std::string> GetTextDocuments(); // метод открывает на чтение все data-файлы и возвращает vector, длина которого равна количеству data-файлов. Используется для записи в InvertedIndex::docs
    std::vector<std::string> GetRequests();
    int GetResponsesLimit();
    void PutAnswers(std::vector<std::vector<std::pair<int, float>>> answers);

	ConverterJSON() = default;
	~ConverterJSON();
	
};

class inverted_index
{
private:
    std::vector<std::string> docs; // каждый индекс соответствует одному файлу. Каждый объект типа string это список слов одного файла
    std::map<std::string, std::vector<entry>> freq_dictionary; 
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
    search_server(inverted_index &idx): _index(idx){};
    std::vector<std::vector<relative_index>> search(const std::vector<std::string>& queries_input);


};
