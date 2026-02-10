#include "sapplication.hpp"

 SApplication::SApplication()
 {
    try
    {	
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
     search_server_ptr = new search_server(inverted_index_ptr);
     
     auto req = ConverterJSON_ptr->GetRequests();
    //for(auto it: req) std::cout << it << std::endl;

 //    inverted_index_ptr->get_word_count(std::string("he"));

    search_server_ptr->search(req);
 }

SApplication::~SApplication()
{

}
