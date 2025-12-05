#include "converter_json.hpp"
#include <fstream>

bool ConverterJSON::check_version()
{
    if(app_version[0] == '.') return false;
    bool last_is_point = false;
    for(int i = 0; i < app_version.length(); ++i)
    {
        if(app_version[i] != '.' && (app_version[i] < 0 || app_version[i] > 0)) return false;
        if(app_version[i] == '.' && last_is_point) return false;
        if(app_version == '.')
            last_is_point = true;
        else
            last_is_point = false;
    }
    std::vector<int> semantic_vers_from_config;
    std::vector<int> semantic_vers_program;

    for(unsigned int i = 0, unsigned int start_pos = 0; i < app_version.length(); ++i)
    {
        if(app_version[i] = '.')
        {
            semantic_vers_from_config.push_back(stoi(app_version.substr(start_pos, i-start_pos-1)));
            start_pos = i + 1;
        }
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
    if (!config.is_open())
    {
        config.close();
        throw;
    }
    else
    {
        config_ifstream >> config_dict;
        app_name = config_dict["config"]["name"];
        app_version = config_dict["config"]["version"];
        config_dict["config"]["max_responses"];
    }
    //std::ifstream
//    config.close();
}

ConverterJSON::~ConverterJSON()
{

}


//class SApplication
//{
//    ConverterJSON* ConverterJSON_ptr;
//    inverted_index* inverted_index_ptr;
//    search_server* search_server_ptr;
//public:
//    SApplication();
//    ~SApplication();
//};

 SApplication::SApplication()
 {
 	try
 	{
 		// вызвать конструктор класса ConverterJSON. Требуется проверка на исключения
        ConverterJSON_ptr = new ConverterJSON;
 	}
 	catch()
 	{

 	}
    catch()
    {

    }

 }
