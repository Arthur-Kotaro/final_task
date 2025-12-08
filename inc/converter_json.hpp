#pragma once

#include <cstddef>
#include <iostream>
#include <string>
#include <vector>
#include <exception>
#include <fstream>

#include "nlohmann/json.hpp"
//#include "gtest/gtest.h"

const std::string exe_version = "0.1";
const std::string conf_path = "config.json";
const std::string answ_path = "answers.json";
const std::string req_path = "requests.json";


class config_file_not_found_exception: std::exception
{
public:
    [[nodiscard]] const char* what() const noexcept override
    {
        return "File \"config.json\" not found";
    }
};


class incorrect_config_file_exception: std::exception
{
public:
    [[nodiscard]] const char* what() const noexcept override
    {
        return "\"config.json\"is empty";
    }
};


class incompatible_config_file_exception: std::exception
{
public:
    [[nodiscard]] const char* what() const noexcept override
    {
        return "Incompatible version of the \"config.json\". Check the configuration or install actual version of the application";
    }
};


class ConverterJSON
{
    nlohmann::json config_dict;
    std::string app_name, app_version;
    unsigned int max_response;
    bool check_version();
    static void txt_version_to_int(std::string str_app_version, std::vector<int> & int_app_version);
public:
    std::vector<std::string> GetTextDocuments();    // метод открывает на чтение data-файлы и возвращает vector строк. Одна строка - один файл. Используется для записи в InvertedIndex::docs
    std::vector<std::string> GetRequests();         // метод открывает на чтение requests.json в каждой строке один запрос (список слов через пробел)
    int GetResponsesLimit();                        // метод возвращает значение поля max_response, содержащего максимум ответов на каждый запрос
    void PutAnswers(std::vector<std::vector<std::pair<int, float>>> answers);

	ConverterJSON();
	~ConverterJSON();
	
};