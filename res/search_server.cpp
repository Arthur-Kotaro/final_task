#include "search_server.hpp"
#define DEBUG

inverted_index::inverted_index(ConverterJSON * _converter_json)
{
    converter_json_ptr = _converter_json;
    update_document_base();
}

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

    for (size_t i = 0; i < docs.size(); ++i)
    {
        index_future.push_back(std::async(std::launch::async, &inverted_index::separate_indexing, static_cast<int>(i), std::cref(docs[i])));
    }
    for (size_t i = 0; i < docs.size(); ++i)
    {
        index_for_each_file.push_back(index_future[i].get());
    }

    //sort vector in descending order

#ifdef DEBUG
    std::cout << "\n\"Before sorting of index_for_each_file vector\"\n";
    size_t sum = 0;
    for(size_t i = 0; i < index_for_each_file.size(); ++i)
    {
        std::cout << index_for_each_file[i]->size() << ' ';
        sum += index_for_each_file[i]->size();
    }
    std::cout << std::endl << "Sum of map.size() " << sum << std::endl;
#endif

    std::sort(begin(index_for_each_file), end(index_for_each_file), [](std::unique_ptr<std::map<std::string, std::vector<entry>>> &left, std::unique_ptr<std::map<std::string, std::vector<entry>>> &right) ->bool
    { return  left->size() > right->size();});

#ifdef DEBUG
    std::cout << "\n\"After sorting of index_for_each_file vector\"\n";
    for(size_t i = 0; i < index_for_each_file.size(); ++i) std::cout << index_for_each_file[i]->size() << ' ';
    std::cout << std::endl;
#endif

    //merge maps and free memory
    merge_separate_map(index_for_each_file, 0, index_for_each_file.size()-1);

#ifdef DEBUG
    std::cout << "\n\"After first merge of index_for_each_file vector\"\n";
    sum = 0;
    for(size_t i = 0; i < index_for_each_file.size(); ++i)
    {
        std::cout << index_for_each_file[i]->size() << ' ';
        sum += index_for_each_file[i]->size();
    }
    std::cout << std::endl << "Sum of map.size() " << sum << std::endl;
#endif

#ifdef DEBUG_words_indexes
//    std::cout << "\n\"Depleted map contains\"\n";
//    for(auto & it : *index_for_each_file[4])
//        std::cout << "Word = " << it.first << ",\tdoc_id = " << it.second[0].doc_id << ", count = " << it.second[0].count << std::endl;
//    std::cout << std::endl;
    std::cout << "******************************************\n******************************************\n******************************************\n";
    std::cout << "\n\"Expanded map contains\"\n";
    for(auto & it : *index_for_each_file[0])
    {
        std::cout << "Word = " << it.first << ",\t";
        for (auto &vec_it:it.second)
        {
            std::cout << "doc_id = " << vec_it.doc_id << ", count = " << vec_it.count << "; ";
        }
        std::cout << std::endl;
    }
#endif

    index_for_each_file.pop_back();

#ifdef DEBUG
    std::cout << "\n\"After first merge of index_for_each_file vector and erase last element\"\n";
    sum = 0;
    for(size_t i = 0; i < index_for_each_file.size(); ++i)
    {
        std::cout << index_for_each_file[i]->size() << ' ';
        sum += index_for_each_file[i]->size();
    }
    std::cout << std::endl << "Sum of map.size() " << sum << std::endl;
#endif
}

size_t inverted_index::merge_separate_map(std::vector<std::unique_ptr<std::map<std::string, std::vector<entry>>>> &sep_index_vec, size_t dst, size_t src)
{
#ifdef DEBUG
    std::cout << "\n\"Function inverted_index::merge_separate_map called. Destination index = " << dst << ", source index = " << src << "\"\n";
#endif

    for (auto it = sep_index_vec[src]->begin();  it != sep_index_vec[src]->end();)
    {
#ifdef DEBUG1
        std::cout << "Current it->first = " << it->first;
#endif
        auto it_next = std::next(it);
        if (!sep_index_vec[dst]->contains(it->first))
        {
#ifdef DEBUG1
            std::cout << ", add to destination" << std::endl;
#endif
            auto tmp = sep_index_vec[src]->extract(it);
            sep_index_vec[dst]->insert(std::move(tmp));
        }
        else
        {
#ifdef DEBUG1
            std::cout << ", add to destinations entry vector" << std::endl;
#endif
            merge_two_sorted_index_vec(sep_index_vec[dst]->operator[](it->first), sep_index_vec[src]->operator[](it->first));
        }
        it = it_next;
    }
    return src;
}

void inverted_index::merge_two_sorted_index_vec(std::vector<entry> &dst, std::vector<entry> &src)
{
    if(src.empty()) return;
    std::sort(src.begin(), src.end(), [](auto const &a, auto const &b) ->bool {return a.doc_id < b.doc_id;}); //exclude statement?
    std::vector<entry> out;
    out.reserve(dst.size() + src.size());
    size_t i = 0, j = 0;
    while ( i < dst.size() && j < src.size())
    {
        if(dst[i].doc_id < src[j].doc_id) out.push_back(std::move(dst[i++]));
        else if (dst[i].doc_id > src[j].doc_id) out.push_back(std::move(src[j++]));
    }
    while (i < dst.size()) out.push_back(std::move(dst[i++]));
    while (j < src.size()) out.push_back(std::move(src[j++]));

    dst = std::move(out);
}


search_server::search_server(inverted_index &idx): _index(idx)
{

}

