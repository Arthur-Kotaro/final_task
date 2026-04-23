#include "converter_json.hpp"
#include <fstream>
#include <iostream>
#include <iterator>

bool RelativeIndex::operator==(const RelativeIndex & other) const
{
  return (docID == other.docID) && (rank == other.rank);
}

void ConverterJSON::TxtVersionToInt(const std::string & str_app_version, std::vector<int> & int_app_version)
{
    unsigned int start_pos = 0;
    for(unsigned int i = 0; i < str_app_version.length(); ++i)
    {
        if(str_app_version[i] == '.')
        {
            int_app_version.push_back(stoi(str_app_version.substr(start_pos, i-start_pos)));
            start_pos = i + 1;
        }
    }
    if(start_pos < str_app_version.length())
        int_app_version.push_back(stoi(str_app_version.substr(start_pos)));
}

bool ConverterJSON::CheckVersion()
{
    if(config_app_version.empty()) return false;
    if(config_app_version[0] == '.') return false;
    bool last_is_point = false;
    for(int i = 0; i < config_app_version.length(); ++i)
    {
        if(config_app_version[i] != '.' && (config_app_version[i] < '0' || config_app_version[i] > '9')) return false;
        if(config_app_version[i] == '.' && last_is_point)                              return false;
        if(config_app_version[i] == '.')
            last_is_point = true;
        else
            last_is_point = false;
    }

    std::vector<int> semantic_vers_from_config;
    std::vector<int> semantic_vers_program;
    TxtVersionToInt(config_app_version, semantic_vers_from_config);
    TxtVersionToInt(app_version, semantic_vers_program);
    size_t version_notation_min_length = (semantic_vers_from_config.size() < semantic_vers_program.size()) ? semantic_vers_from_config.size() : semantic_vers_program.size();
    for (size_t i = 0; i < version_notation_min_length; ++i)
    {
        if (semantic_vers_program[i] < semantic_vers_from_config[i]) return false;
    }
    return semantic_vers_program.size() >= semantic_vers_from_config.size();
}

std::vector<std::string> ConverterJSON::GetTextDocuments() const
{
	std::vector<std::string> text_documents;
	for(const auto & file : files)
	{
		std::ifstream txt_file;
		txt_file.open(file, std::ios::binary);
		if (txt_file.is_open())
		{
			text_documents.emplace_back(std::istreambuf_iterator<char>(txt_file), std::istreambuf_iterator<char>());
        }
        else
        {
            std::cerr << "Warning: file \"" << file << "\" can\'t be open." << std::endl;
        }
        txt_file.close();
    }
    return text_documents;
}

std::string& ConverterJSON::GetName()
{
    return config_app_name;
}

std::vector<std::string> ConverterJSON::GetRequests()
{
    std::vector<std::string> requests;
    std::ifstream request_ifstream(req_path);
    if (request_ifstream.is_open())
    {
   		request_ifstream >> requests_dict;
        if(requests_dict.contains("requests") && !requests_dict["requests"].is_null())
            requests = requests_dict["requests"];
        else
            std::cerr << "Warning: file \"" << req_path << "\" is empty." << std::endl;
    }
    else
    {
        std::cerr << "Warning: file \"" << req_path << "\" can\'t be open." << std::endl; 
    }
    request_ifstream.close();
    return requests;
}

unsigned int ConverterJSON::GetResponsesLimit() const
{
    return max_responses;
}

ConverterJSON::ConverterJSON()
{
    std::ifstream config_ifstream(conf_path);
    if (!config_ifstream.is_open())
    {
        config_ifstream.close();
        throw config_file_not_found_exception();
    }
    config_ifstream >> config_dict;
    if(!config_dict.contains("config") && config_dict["config"].is_null())
    {
        throw empty_config_file_exception();
    }
    if(config_dict["config"].contains("name") && !config_dict["config"]["name"].is_null())
        config_app_name = config_dict["config"]["name"];
    if(config_dict["config"].contains("version") && !config_dict["config"]["version"].is_null())
    {
        config_app_version = config_dict["config"]["version"];
        if(!CheckVersion()) 
	        throw incompatible_config_file_exception();
    }
    if(config_dict.contains("max_responses") && !config_dict["max_responses"].is_null())
        max_responses = config_dict["config"]["max_responses"];
    else
        max_responses = DEFAULT_MAX_RESPONSE;

    if(config_dict.contains("update_interval") && !config_dict["update_interval"].is_null())
        update_interval = config_dict["config"]["update_interval"];
    else
        update_interval = DEFAULT_UPDATE_INTERVAL;
    files = config_dict["files"]; 
    config_ifstream.close();
}

static nlohmann::json RelativeIndexToJSON(const RelativeIndex & relevance)
{
	return nlohmann::json{{"docID", relevance.docID}, {"rank", relevance.rank}};
}

void ConverterJSON::PutAnswers(const std::vector<std::vector<RelativeIndex>> & answers)
{
	using nlohmann::json;
    if(answers.empty())
    {
        std::cerr << "Warning: there is nothing to save." << std::endl;
        return;
    }

	std::ofstream AnswersFile(answ_path);
	if(!AnswersFile.is_open() || answers.empty()) return;
	json resultJSON;
	resultJSON["answers"] = json::object();
	int requestNumber = 1;
	for(const auto & requestAnswer : answers)
	{
		json answerJSON;
		json documentList = json::array();
		for(const auto & relevantDocument : requestAnswer)
		{
			documentList.push_back(RelativeIndexToJSON(relevantDocument));
		}
		if(documentList.empty())
		{
			answerJSON["result"] = false;
		}
		else
		{
			answerJSON["result"] = true;
			answerJSON["relevance"] = std::move(documentList);
		}
		char keybuf[16];
		std::snprintf(keybuf, sizeof(keybuf), "request%03d", requestNumber);
		resultJSON["answers"][keybuf] = std::move(answerJSON);
		requestNumber++;
	}
	AnswersFile << resultJSON.dump(4) << std::endl;
	AnswersFile.close();
}

ConverterJSON::~ConverterJSON()
{

}
