#include "sapplication.hpp"

 SApplication::SApplication()
 {
    try
    {	
        ConverterJSONPtr = new ConverterJSON;
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
     InvertedIndexPtr = new InvertedIndex(ConverterJSONPtr);
     SearchServerPtr = new SearchServer(InvertedIndexPtr);
     
     auto req = ConverterJSONPtr->GetRequests();
    //for(auto it: req) std::cout << it << std::endl;

 //    inverted_index_ptr->get_word_count(std::string("he"));
     ConverterJSONPtr->PutAnswers(SearchServerPtr->Search(req));
 }

SApplication::~SApplication()
= default;
