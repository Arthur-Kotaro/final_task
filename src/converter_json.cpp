#include "converter_json.hpp"


bool RelativeIndex::operator==(const RelativeIndex & other) const
{
  return (docID == other.docID) && (rank == other.rank);
}


void ConverterJSON::TxtVersionToInt(const std::string str_app_version, std::vector<int> & int_app_version)
{
    unsigned int start_pos = 0;
    for(unsigned int i = 0; i < str_app_version.length(); ++i)
    {
        if(str_app_version[i] == '.')
        {
            int_app_version.push_back(stoi(str_app_version.substr(start_pos, i-start_pos-1)));
            start_pos = i + 1;
        }
    }
}

bool ConverterJSON::CheckVersion()
{
    if(app_version[0] == '.') return false;
    bool last_is_point = false;
    for(int i = 0; i < app_version.length(); ++i)
    {
        if(app_version[i] != '.' && (app_version[i] < 0 || app_version[i] > 0)) return false;
        if(app_version[i] == '.' && last_is_point)                              return false;
        if(app_version[i] == '.')
            last_is_point = true;
        else
            last_is_point = false;
    }

    std::vector<int> semantic_vers_from_config;
    std::vector<int> semantic_vers_program;
    TxtVersionToInt(app_version, semantic_vers_from_config);
    TxtVersionToInt(exe_version, semantic_vers_program);

//    bool app_newer_than_config = false;
    unsigned int version_notation_min_length = (semantic_vers_from_config.size() < semantic_vers_program.size()) ? semantic_vers_from_config.size() : semantic_vers_program.size();
    for (int i = 0; i < version_notation_min_length; ++i)
    {
        if (semantic_vers_program[i] < semantic_vers_from_config[i]) return false;
    }
    return true;
}


std::vector<std::string> ConverterJSON::GetTextDocuments()
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
        txt_file.close();
    	}
    return text_documents;
}


std::string& ConverterJSON::GetName()
{
    return app_name;
}


std::vector<std::string> ConverterJSON::GetRequests()
{
    std::vector<std::string> requests;
    std::ifstream request_ifstream(req_path);
    if (!request_ifstream.is_open())
    {
   		request_ifstream.close();
      std::exit(0);
    }
    request_ifstream >> requests_dict;
    if(!requests_dict.contains("requests") && config_dict["requests"].is_null())
    {
#ifdef DEBUG_REQ
        std::cout << "requests.json is empty" << std::endl;
#endif
            //throw empty_config_file_exception();
    }
    else
    {
	requests = requests_dict["requests"];
    }
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
    if(config_dict.contains("name") && !config_dict["name"].is_null())
        app_name = config_dict["config"]["name"];
    if(config_dict.contains("version") && !config_dict["version"].is_null())
    {
        app_version = config_dict["config"]["version"];
        if(!CheckVersion())
	{
	    throw incompatible_config_file_exception(); 
	}
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
