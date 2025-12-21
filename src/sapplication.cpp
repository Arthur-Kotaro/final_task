#include "sapplication.hpp"

 SApplication::SApplication()
 {
 	try
 	{
 		// вызвать конструктор класса ConverterJSON. Требуется проверка на исключения
        ConverterJSON_ptr = new ConverterJSON;
 	}
    catch(const config_file_not_found_exception & except)
    {
        std::cerr << except.what();
    }
    catch(const empty_config_file_exception & except)
    {
        std::cerr << except.what();
    }
    catch(const incompatible_config_file_exception & except)
    {
        std::cerr << except.what();
    }
     inverted_index_ptr = new inverted_index(ConverterJSON_ptr);
     search_server_ptr = new search_server(*inverted_index_ptr);
 }
