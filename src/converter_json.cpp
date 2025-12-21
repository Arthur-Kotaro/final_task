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


std::vector<std::string> ConverterJSON::GetRequests()
{
    std::vector<std::string> requests;
    /*
     *
     */
    return requests;
}

unsigned int ConverterJSON::GetResponsesLimit() const
{
    return max_response;
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
        if(!config_dict.contains("config") && config_dict["config"].is_null())
        {
            throw empty_config_file_exception();
        }
        if(config_dict.contains("name") && !config_dict["name"].is_null())
            app_name = config_dict["config"]["name"];
        if(config_dict.contains("version") && !config_dict["version"].is_null())
        {
            app_version = config_dict["config"]["version"];
            if(!check_version())    throw incompatible_config_file_exception();
        }

        if(config_dict.contains("max_responses") && !config_dict["max_responses"].is_null())
            max_response = config_dict["config"]["max_responses"];
        else
            max_response = DEFAULT_MAX_RESPONSE;

        if(config_dict.contains("update_interval") && !config_dict["update_interval"].is_null())
            update_interval = config_dict["config"]["update_interval"];
        else
            update_interval = DEFAULT_UPDATE_INTERVAL;
        files = config_dict["files"];
    }
    config_ifstream.close();
}

ConverterJSON::~ConverterJSON()
{

}