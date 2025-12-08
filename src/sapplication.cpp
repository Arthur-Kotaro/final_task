#include "sapplication.hpp"


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
 	catch(const incompatible_config_file_exception & except)
 	{
        std::cerr << except.what();
 	}
    catch()
    {

    }
     inverted_index_ptr = new inverted_index;
 }
