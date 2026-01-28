#include "search_server.hpp"


inverted_index::inverted_index(ConverterJSON * _converter_json)
{
//    converter_json_ptr = _converter_json;
    update_document_base(_converter_json->GetTextDocuments());
}



void inverted_index::update_document_base(std::vector<std::string> input_docs)
{
    docs = std::move(input_docs);
    if(docs.empty()) return;
//    if(!freq_dictionary->empty()) freq_dictionary->clear();
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
    std::sort(begin(index_for_each_file), end(index_for_each_file), [](std::unique_ptr<std::map<std::string, std::vector<entry>>> &left, std::unique_ptr<std::map<std::string, std::vector<entry>>> &right) ->bool
    { return  left->size() > right->size();});
    //merge maps and free memory
    const unsigned int hardware_threads = std::thread::hardware_concurrency() ? std::thread::hardware_concurrency() : 2;
    merge_auxiliary_maps(index_for_each_file, hardware_threads);
    freq_dictionary = std::move(index_for_each_file[0]);
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



void inverted_index::merge_auxiliary_maps(std::vector<std::unique_ptr<std::map<std::string, std::vector<entry>>>> &auxiliary_maps, const unsigned int hardware_threads)
{
    if (auxiliary_maps.size() == 1) return;
    unsigned int threads_num = (auxiliary_maps.size() / 2) >= hardware_threads ? hardware_threads : (auxiliary_maps.size() / 2);
    std::vector<std::thread> threads;
    threads.reserve(threads_num);
    for (size_t i = 0; i < threads_num; ++i)
    {
        threads.emplace_back(merge_two_separated_maps, std::ref(auxiliary_maps), i, auxiliary_maps.size() - i - 1);
    }
    for (auto &it: threads)
    {
        if (it.joinable()) it.join();
    }
    for (int i = 0; i < threads_num; ++i)
    {
        auxiliary_maps.pop_back();
    }
    if (auxiliary_maps.size() > 1)
        merge_auxiliary_maps(auxiliary_maps, hardware_threads);
}



void inverted_index::merge_two_separated_maps(std::vector<std::unique_ptr<std::map<std::string, std::vector<entry>>>> &sep_index_vec, size_t dst, size_t src)
{
    for (auto it = sep_index_vec[src]->begin();  it != sep_index_vec[src]->end();)
    {
        auto it_next = std::next(it);
        if (!sep_index_vec[dst]->contains(it->first))
        {
            auto tmp = sep_index_vec[src]->extract(it);
            sep_index_vec[dst]->insert(std::move(tmp));
        }
        else
            merge_two_sorted_index_vec(sep_index_vec[dst]->operator[](it->first), sep_index_vec[src]->operator[](it->first));
        it = it_next;
    }
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
    while (i < dst.size()) out.push_back(dst[i++]);
    while (j < src.size()) out.push_back(src[j++]);
    dst = std::move(out);
}



std::vector<entry>& inverted_index::get_word_count(const std::string &word)
{
	auto it = freq_dictionary->find(word);
	return it->second;
}



std::vector<std::vector<relative_index>> search_server::search(const std::vector<std::string>& queries_input)
{
	std::vector<std::list<search_term>> search_requests;
	search_requests.reserve(queries_input.size());
	for(const auto & line: queries_input)
	{	
		if(!line.empty())
		{
			std::list<search_term> lst;
			const char *start    = line.c_str();
    			const char *end      = start + line.size();
    			const char *iterator = start;
    			while (iterator < end)
    			{
        			while (iterator < end && ( (*iterator < 'a') || (*iterator > 'z') )) ++iterator;
        			start = iterator;
        			while (iterator < end && ( (*iterator >= 'a') && (*iterator <= 'z') )) ++iterator;
				std::string word(start, iterator);
			
				auto it = std::find_if(lst.begin(), lst.end(), [&](const search_term & trm) { return trm.term == word;});
				if(it == lst.end())
				{
					auto tmp = search_term { word, ([&word, this]() -> int { 
						int num = 0;					
//						_index->get_word_count(std::string("he"));
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!					
//						for(auto iter : _index->get_word_count(word))

						const auto & entry_vec = _index->get_word_count(word); //ERROR!!!
						for(const auto & iter : entry_vec)
							num += iter.count;
						return num;
						})()};
					bool inserted = false;
					for(auto iter = lst.begin(); iter != lst.end(); ++iter)
					{
						if(iter->count >= tmp.count)
						{
							lst.insert(iter, tmp);
							inserted = true;
							break;
						}
					}
					if(!inserted) lst.push_back(tmp);
				}
			}
			search_requests.push_back(std::move(lst));
		}
	}
	std::vector<std::vector<relative_index>> ret = {{{1, 2},{3, 4}}, {{5, 6}}};
	return ret;
}

