#include "sapplication.hpp"
#include <iostream>

SApplication::SApplication()
{
   try
   {	
       ConverterJSON_ptr = new ConverterJSON;
   }
   catch(const config_file_not_found_exception & except)
   {
       std::cerr << except.what();
       std::exit(0);
   }
   catch(const empty_config_file_exception & except)
   {
       std::cerr << except.what();
       std::exit(0);
   }
   catch(const incompatible_config_file_exception & except)
   {
       std::cerr << except.what();
       std::exit(0);
   }
   std::cout << "Starting " << ConverterJSON_ptr->GetName() << std::endl;
   InvertedIndex_ptr = new InvertedIndex(ConverterJSON_ptr);
   SearchServer_ptr = new SearchServer(InvertedIndex_ptr);
   ConverterJSON_ptr->PutAnswers(SearchServer_ptr->Search(ConverterJSON_ptr->GetRequests()));
}

SApplication::~SApplication()
{

}
