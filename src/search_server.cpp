#include "search_server.hpp"
#define DEBUG

inverted_index::inverted_index(ConverterJSON * _converter_json)
{
    converter_json_ptr = _converter_json;
    update_document_base();
}

//std::map<std::string, std::vector<entry>> * inverted_index::separate_indexing(int _doc_id, const std::string & txt_file_content)
std::unique_ptr<std::map<std::string, std::vector<entry>>> inverted_index::separate_indexing(int _doc_id, const std::string & txt_file_content)

{
    auto separated_map = std::make_unique<std::map<std::string, std::vector<entry>>>();
    const char *start    = txt_file_content.c_str();
    const char *end      = start + txt_file_content.size();
    const char *iterator = start;
    while (iterator < end)
    {
        while (iterator < end && ( (*iterator < 'a') || (*iterator > 'z') )) ++iterator; //skip all symbols except alphabet letters
        start = iterator;
        while (iterator < end && ( (*iterator >= 'a') && (*iterator <= 'z') )) ++iterator;
//        if(start == iterator) continue;
        std::string word(start, iterator);
        auto map_iterator = separated_map->find(word);
        if (map_iterator == separated_map->end())
            separated_map->emplace(word, std::vector<entry> { entry{ static_cast<size_t>(_doc_id), 1 } }); //add new word in the map
        else
            map_iterator->second[0].count++; //incrementation
    }
    return separated_map;
}


void inverted_index::update_document_base()
{
#ifdef DEBUG
    std::cout << "\n\"inverted_index::update_document_base() \" called\n";
#endif

    docs = converter_json_ptr->GetTextDocuments();
    std::vector<std::future<std::unique_ptr<std::map<std::string, std::vector<entry>>>>> index_future;
    std::vector<std::unique_ptr<std::map<std::string, std::vector<entry>>>> index_for_each_file;
    index_future.reserve(docs.size());
    index_for_each_file.reserve(docs.size());


//    std::string example("some text");
//    int ex = 5;
    for (size_t i = 0; i < docs.size(); ++i)
    {
//        index_future.push_back(std::async(std::launch::async, &inverted_index::separate_indexing, ex, example));
        index_future.push_back(std::async(std::launch::async, &inverted_index::separate_indexing, static_cast<int>(i), std::cref(docs[i])));
//        index_future.push_back(std::async(std::launch::async, &inverted_index::separate_indexing, this, static_cast<int>(i), std::cref(docs[i])));

//        index_for_each_file_ftr[i] = std::async(std::launch::async, [this, &i]() -> std::future<std::map<std::string, std::vector<entry>> *> { return separate_indexing(i, docs[i]);});
    }
    for (size_t i = 0; i < docs.size(); ++i)
    {
        index_for_each_file.push_back(index_future[i].get());
    }

    //sort vector in ascending order

#ifdef DEBUG
    std::cout << "\n\"Sorting of index_for_each_file array\"\n";
    for(size_t i = 0; i < docs.size(); ++i) std::cout << index_for_each_file[i]->size() << ' ';
    std::cout << std::endl;
#endif

    std::sort(begin(index_for_each_file), end(index_for_each_file), [](std::unique_ptr<std::map<std::string, std::vector<entry>>> &left, std::unique_ptr<std::map<std::string, std::vector<entry>>> &right) ->bool
    { return  left->size() > right->size();});

#ifdef DEBUG
    std::cout << "\n\"Sorting of index_for_each_file array\"\n";
    for(size_t i = 0; i < docs.size(); ++i) std::cout << index_for_each_file[i]->size() << ' ';
    std::cout << std::endl;
#endif

    //merge maps and free memory

//    for(size_t i = 0; i < docs.size()-1; ++i)
//    {
//        delete index_for_each_file[i];
//    }
//    freq_dictionary = index_for_each_file[docs.size()-1];
}


search_server::search_server(inverted_index &idx): _index(idx)
{

}

