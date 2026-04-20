#pragma once

#include <string>
#include <vector>
#include "nlohmann/json.hpp"

#define DEFAULT_MAX_RESPONSE 5
#define DEFAULT_UPDATE_INTERVAL 5

const std::string exe_version = "0.1";
const std::string conf_path = "config.json";
const std::string answ_path = "answers.json";
const std::string req_path = "requests.json";

struct RelativeIndex
{
    size_t docID;
    float rank;
    bool operator==(const RelativeIndex & other) const;
};

class config_file_not_found_exception: std::exception
{
public:
    [[nodiscard]] const char* what() const noexcept override
    {
        return "File \"config.json\" is missing";
    }
};


class empty_config_file_exception: std::exception
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
        return "Incompatible version of the \"config.json\". Check the configuration or install an actual version of the application";
    }
};


class ConverterJSON
{
    nlohmann::json config_dict, requests_dict;
    std::string app_name, app_version;
    unsigned int max_responses, update_interval;
    std::vector<std::string> files;

    bool CheckVersion();
    static void TxtVersionToInt(std::string str_app_version, std::vector<int> & int_app_version);

public:
    [[nodiscard]] std::vector<std::string> GetTextDocuments() const;
    std::vector<std::string> GetRequests();
    std::string& GetName();
    [[nodiscard]] unsigned int GetResponsesLimit() const;
    void PutAnswers(const std::vector<std::vector<RelativeIndex>> & answers);

	ConverterJSON();
	~ConverterJSON();
};