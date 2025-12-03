#include "converter_json.hpp"

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
    std::ifstream config;
    config.open(conf_path);
    if (!config.is_open())
    {
        config.close();
        throw;
    }

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