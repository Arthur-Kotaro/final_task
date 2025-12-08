#include "converter_json.hpp"

void ConverterJSON::txt_version_to_int(const std::string str_app_version, std::vector<int> & int_app_version)
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

bool ConverterJSON::check_version()
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
    txt_version_to_int(app_version, semantic_vers_from_config);
    txt_version_to_int(exe_version, semantic_vers_program);

//    bool app_newer_than_config = false;
    unsigned int version_notation_min_length = (semantic_vers_from_config.size() < semantic_vers_program.size()) ? semantic_vers_from_config.size() : semantic_vers_program.size();
    for (int i = 0; i < version_notation_min_length; ++i)
    {
        if (semantic_vers_program[i] < semantic_vers_from_config[i]) return false;
//        if (semantic_vers_program[i] > semantic_vers_from_config[i]) app_newer_than_config = true;
    }
    return true;
}


std::vector<std::string> ConverterJSON::GetTextDocuments()
{

}


std::vector<std::string> ConverterJSON::GetRequests()
{

}

int ConverterJSON::GetResponsesLimit()
{

}


void ConverterJSON::PutAnswers(std::vector<std::vector<std::pair<int, float>>> answers)
{


}

ConverterJSON::ConverterJSON()
{
    std::ifstream config_ifstream(conf_path);
    if (!config_ifstream.is_open())
    {
        config_ifstream.close();
        throw config_file_not_found_exception();
    }
    else
    {
        config_ifstream >> config_dict;
        app_name = config_dict["config"]["name"];
        app_version = config_dict["config"]["version"];
        config_dict["config"]["max_responses"];
//        if()                    throw incorrect_config_file_exception(); если в config_dict["config"] ничего нет
        if(!check_version())    throw incompatible_config_file_exception();
    }
    config_ifstream.close();
}

ConverterJSON::~ConverterJSON()
{

}