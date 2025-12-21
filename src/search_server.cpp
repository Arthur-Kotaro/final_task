#include "search_server.hpp"
#define DEBUG

inverted_index::inverted_index(ConverterJSON * _converter_json)
{
    converter_json_ptr = _converter_json;
    update_document_base();
}

std::map<std::string, std::vector<entry>> * inverted_index::separate_indexing(int _doc_id, const std::string & txt_file_content)
{
    auto separated_map = new std::map<std::string, std::vector<entry>>;
    const char *iterator = txt_file_content.c_str();
    const char *end = iterator + txt_file_content.size();
    const char *start = iterator;
    while (iterator < end)
    {
        while (iterator < end && ( (*iterator < 'a') || (*iterator > 'z') )) ++iterator; //skip all symbols except alphabet letters
        start = iterator;
        while (iterator < end && ( (*iterator >= 'a') && (*iterator <= 'z') )) ++iterator;
        std::string word(start, iterator);
        auto map_iterator = separated_map->find(word);
        if (map_iterator == separated_map->end())
        {
            separated_map->emplace(word, std::vector<entry> { entry{ static_cast<size_t>(_doc_id), 1 } }); //add new word in the map
        }
        else
        {
            map_iterator->second[0].count++; //incrementation
        }
    }
    return separated_map;
}


void inverted_index::update_document_base()
{
#ifdef DEBUG
    std::cout << "\n\"inverted_index::update_document_base() \" called\n";
#endif

    docs = converter_json_ptr->GetTextDocuments();
    std::map<std::string, std::vector<entry>> * index_for_each_file[docs.size()];
    for (int i = 0; i < docs.size(); ++i)
    {
        index_for_each_file[i] = separate_indexing(i, docs[i]);
    }
    //sort array in ascending order
    std::map<std::string, std::vector<entry>> * biggest = index_for_each_file[docs.size()-1];
    std::map<std::string, std::vector<entry>> * tmp;

#ifdef DEBUG
    std::cout << "\n\"Sorting of index_for_each_file array\"\n";
    for(int i = 0; i < docs.size(); ++i)
    {
        std::cout << index_for_each_file[i]->size() << ' ';
    }
    std::cout << std::endl;
#endif

    for (int i = docs.size(); i > 0; --i)
    {
        if(index_for_each_file[i]->size() > biggest->size())
        {
            tmp = biggest;
            biggest = index_for_each_file[i];
            index_for_each_file[i] = tmp;
        }
    }

#ifdef DEBUG
    std::cout << "\n\"Sorting of index_for_each_file array\"\n";
    for(int i = 0; i < docs.size(); ++i)
    {
        std::cout << index_for_each_file[i]->size() << ' ';
    }
    std::cout << std::endl;
#endif
    //merge maps and free memory
    for(int i = 0; i < docs.size()-1; ++i)
    {
        delete index_for_each_file[i];
    }
    freq_dictionary = index_for_each_file[docs.size()-1];


}





search_server::search_server(inverted_index &idx): _index(idx)
{

}

