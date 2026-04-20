#include "search_server.hpp"
#include <future>

bool Entry::operator==(const Entry & other) const
{
  return (docID == other.docID) && (count == other.count);
}

InvertedIndex::InvertedIndex(ConverterJSON * _ConverterJSON)
{
    UpdateDocumentBase(_ConverterJSON->GetTextDocuments());
}

void InvertedIndex::UpdateDocumentBase(std::vector<std::string> input_docs)
{
    docs = std::move(input_docs);
    if(docs.empty()) return;
    std::vector<std::future<std::unique_ptr<std::map<std::string, std::vector<Entry>>>>> index_future;
    std::vector<std::unique_ptr<std::map<std::string, std::vector<Entry>>>> index_for_each_file;
    index_future.reserve(docs.size());
    index_for_each_file.reserve(docs.size());

    for (size_t i = 0; i < docs.size(); ++i)
    {
        index_future.push_back(std::async(std::launch::async, &InvertedIndex::SeparateIndexing, static_cast<int>(i), std::cref(docs[i])));
    }
    for (size_t i = 0; i < docs.size(); ++i)
    {
        index_for_each_file.push_back(index_future[i].get());
    }

    //sort vector in descending order
    std::sort(begin(index_for_each_file), end(index_for_each_file), [](std::unique_ptr<std::map<std::string, std::vector<Entry>>> &left, std::unique_ptr<std::map<std::string, std::vector<Entry>>> &right) ->bool
    { return  left->size() > right->size();});
    //merge maps and free memory
    const unsigned int hardware_threads = std::thread::hardware_concurrency() ? std::thread::hardware_concurrency() : 2;
    MergeAuxiliaryMaps(index_for_each_file, hardware_threads);
    freq_dictionary = std::move(index_for_each_file[0]);
}

std::unique_ptr<std::map<std::string, std::vector<Entry>>> InvertedIndex::SeparateIndexing(int _docID, const std::string & txt_file_content)
{
    auto separated_map = std::make_unique<std::map<std::string, std::vector<Entry>>>();
    const char *start    = txt_file_content.c_str();
    const char *end      = start + txt_file_content.size();
    const char *iterator = start;
    while (iterator < end)
    {
        while (iterator < end && ( (*iterator < 'a') || (*iterator > 'z') )) ++iterator; //skip all symbols except alphabet letters
        start = iterator;
        while (iterator < end && ( (*iterator >= 'a') && (*iterator <= 'z') )) ++iterator;
        if(start == iterator) continue;
        std::string word(start, iterator);
        auto map_iterator = separated_map->find(word);
        if (map_iterator == separated_map->end())
            separated_map->emplace(word, std::vector<Entry> { Entry{ static_cast<size_t>(_docID), 1 } }); //add new word in the map
        else
            map_iterator->second[0].count++; //incrementation
    }
    return separated_map;
}

void InvertedIndex::MergeAuxiliaryMaps(std::vector<std::unique_ptr<std::map<std::string, std::vector<Entry>>>> &auxiliary_maps, const unsigned int hardware_threads)
{
    if (auxiliary_maps.size() == 1) return;
    unsigned int threads_num = (auxiliary_maps.size() / 2) >= hardware_threads ? hardware_threads : (auxiliary_maps.size() / 2);
    std::vector<std::thread> threads;
    threads.reserve(threads_num);
    for (size_t i = 0; i < threads_num; ++i)
    {
        threads.emplace_back(MergeTwoSeparatedMaps, std::ref(auxiliary_maps), i, auxiliary_maps.size() - i - 1);
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
        MergeAuxiliaryMaps(auxiliary_maps, hardware_threads);
}

void InvertedIndex::MergeTwoSeparatedMaps(std::vector<std::unique_ptr<std::map<std::string, std::vector<Entry>>>> &sep_index_vec, size_t dst, size_t src)
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
            MergeTwoSortedIndexVec(sep_index_vec[dst]->operator[](it->first), sep_index_vec[src]->operator[](it->first));
        it = it_next;
    }
}

void InvertedIndex::MergeTwoSortedIndexVec(std::vector<Entry> &dst, std::vector<Entry> &src)
{
    if(src.empty()) return;
    std::sort(src.begin(), src.end(), [](auto const &a, auto const &b) ->bool {return a.docID < b.docID;}); //exclude statement?
    std::vector<Entry> out;
    out.reserve(dst.size() + src.size());
    size_t i = 0, j = 0;
    while ( i < dst.size() && j < src.size())
    {
        if(dst[i].docID < src[j].docID) out.push_back(std::move(dst[i++]));
        else if (dst[i].docID > src[j].docID) out.push_back(std::move(src[j++]));
    }
    while (i < dst.size()) out.push_back(dst[i++]);
    while (j < src.size()) out.push_back(src[j++]);
    dst = std::move(out);
}

std::vector<Entry> InvertedIndex::GetWordCount(const std::string &word) const
{
	auto it = freq_dictionary->find(word);
  if(it == freq_dictionary->end()) return {};
	return it->second;
}

std::vector<std::list<SearchTerm>> SearchServer::ParseRequest(const std::vector<std::string>& queries_input) const
{
    std::vector<std::list<SearchTerm>> parsed_requests(queries_input.size());
    for (size_t lineIndex = 0; lineIndex < queries_input.size(); ++lineIndex)
    {
        if(!queries_input[lineIndex].empty())
        {
            std::list<SearchTerm> lst;
            const char *start    = queries_input[lineIndex].c_str();
            const char *end      = start + queries_input[lineIndex].size();
            const char *iterator = start;
            while (iterator < end)
            {
                //Parsing input string
                while (iterator < end && !((*iterator >= 'a' && *iterator <= 'z') || (*iterator >= 'A' && *iterator <= 'Z'))) ++iterator;
                start = iterator;
                while (iterator < end && ((*iterator >= 'a' && *iterator <= 'z') || (*iterator >= 'A' && *iterator <= 'Z'))) ++iterator;
                if(start == iterator)
                {
                    iterator++;
                    continue;
                }
                std::string word(start, iterator);
                std::transform(word.begin(), word.end(), word.begin(), [](const unsigned char c){ return static_cast<unsigned char>(std::tolower(c)); });
                auto it = std::find_if(lst.begin(), lst.end(), [&](const SearchTerm & trm) { return trm.term == word;});
                if(it == lst.end())
                {
                    auto tmp = SearchTerm { word, ([&word, this]() -> size_t {
                        size_t cnt = 0;
                        auto entry_vec = _index->GetWordCount(word);
                        for (auto &e: entry_vec) cnt += e.count;
                        return cnt;
                    })()};
                    if (tmp.count == 0) continue;
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
            parsed_requests[lineIndex] = (std::move(lst));
        }
        else
            parsed_requests[lineIndex] = std::list<SearchTerm>();
    }
    return parsed_requests;
}

size_t SearchServer::FindMaxAbsoluteRelevance(const std::list<AbsoluteIndex> & doc_list)
{
    size_t max = 0;
    if(!doc_list.empty())
    {
        for(auto it : doc_list)
        {
            if (it.absolute_relevance > max) max = it.absolute_relevance;
        }
    }
    return max;
}

std::vector<std::vector<RelativeIndex>> SearchServer::Search(const std::vector<std::string>& queries_input) const
{
    std::vector<std::list<SearchTerm>> parsed_requests = ParseRequest(queries_input);
    std::vector<std::list<AbsoluteIndex>> relevant_docs(parsed_requests.size());

    for(size_t request = 0; request < parsed_requests.size(); ++request)
    {
        if (!parsed_requests[request].empty())
        {
            const auto & entry_vec = _index->GetWordCount(parsed_requests[request].begin().operator->()->term);
            std::list<AbsoluteIndex> relevant_docs_list;
            if (!entry_vec.empty())
            {
                for (const auto term_entry: entry_vec)
                {
                    relevant_docs_list.emplace_back(AbsoluteIndex{term_entry.docID, term_entry.count});
                }
                for (auto & search_word = ++(parsed_requests[request].begin()); search_word != parsed_requests[request].end() ; ++search_word)
                {
                    const auto & entries = _index->GetWordCount(search_word.operator->()->term);
                    if (!entries.empty())
                    {
                        for (auto & iter : entries)
                        {
                            for (auto & relevant_doc : relevant_docs_list)
                            {
                                if (iter.docID == relevant_doc.docID)
                                {
                                    relevant_doc.absolute_relevance += iter.count;
                                    break;
                                }
                            }
                        }
                    }
                }

            }
            relevant_docs[request] = std::move(relevant_docs_list);
        }
    }
    //sort relevant_docs in descending order
    for (auto & lst : relevant_docs)
    {
        lst.sort([](const AbsoluteIndex & a, const AbsoluteIndex & b)
            {
            return a.absolute_relevance > b.absolute_relevance;
            });
    }
    std::vector<std::vector<RelativeIndex>> result(relevant_docs.size());
    //count relative relevance
    for (size_t list_idx = 0; list_idx < relevant_docs.size(); ++list_idx)
    {
        size_t max = FindMaxAbsoluteRelevance(relevant_docs[list_idx]);
        if (max)
        {
            for(auto & doc : relevant_docs[list_idx])
            {
                result[list_idx].emplace_back(RelativeIndex{doc.docID, static_cast<float>(doc.absolute_relevance) / static_cast<float>(max)});
            }
        }
    }
	return result;
}