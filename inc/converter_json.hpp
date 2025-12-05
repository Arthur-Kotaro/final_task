#pragma once

#include <cstddef>
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <exception>
#include <fstream>

#include "nlohmann/json.hpp"
//#include "gtest/gtest.h"

const std::string program_version = "0.1";
const std::string conf_path = "config.json";
const std::string answ_path = "answers.json";
const std::string req_path = "requests.json";


struct entry
{
    std::size_t doc_id, count;
};

struct relative_index
{
    size_t doc_id;
    float rank;
};



class config_file_not_found_exception: std::exception
{
    const char* what() const noexcept override
    {
        return "File /"config.json/" not found";
    }
};


class incorrect_config_file_exception: std::exception
{
    const char* what() const noexcept override
    {
        return "/"config.json/"is empty";
    }
};


class incompatible_config_file_exception: std::exception
{
    const char* what() const noexcept override
    {
        return "Incompatible version of the /"config.json/". Check the configuration or install actual version of the application";
    }
}




class ConverterJSON
{
    nlohmann::json config_dict;
    std::string app_name, app_version;
    unsigned int max_response;
    bool check_version();
public:
    std::vector<std::string> GetTextDocuments();    // метод открывает на чтение data-файлы и возвращает vector строк. Одна строка - один файл. Используется для записи в InvertedIndex::docs
    std::vector<std::string> GetRequests();         // метод открывает на чтение requests.json в каждой строке один запрос (список слов через пробел)
    int GetResponsesLimit();                        // метод возвращает значение поля max_response, содержащего максимум ответов на каждый запрос
    void PutAnswers(std::vector<std::vector<std::pair<int, float>>> answers);

	ConverterJSON();
	~ConverterJSON();
	
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
    search_server(inverted_index &idx): _index(idx){};
    std::vector<std::vector<relative_index>> search(const std::vector<std::string>& queries_input);


};

class SApplication
{
    ConverterJSON* ConverterJSON_ptr;
    inverted_index* inverted_index_ptr;
    search_server* search_server_ptr;
public:
    SApplication();
    ~SApplication();
};
