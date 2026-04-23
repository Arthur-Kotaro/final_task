#include "search_server.hpp"
#include <iostream>
#include <future>
#include <thread>

bool Entry::operator==(const Entry & other) const
{
    return (docID == other.docID) && (count == other.count);
}

InvertedIndex::InvertedIndex() : freq_dictionary(nullptr) {}

InvertedIndex::InvertedIndex(ConverterJSON* _ConverterJSON) : freq_dictionary(nullptr)
{
    UpdateDocumentBase(_ConverterJSON->GetTextDocuments());
}

InvertedIndex::~InvertedIndex()
{
    delete freq_dictionary;
}

InvertedIndex::InvertedIndex(InvertedIndex&& other) noexcept
    : docs(std::move(other.docs))
    , freq_dictionary(other.freq_dictionary)
{
    other.freq_dictionary = nullptr;
}

InvertedIndex& InvertedIndex::operator=(InvertedIndex&& other) noexcept
{
    if (this != &other)
    {
        delete freq_dictionary;
        docs = std::move(other.docs);
        freq_dictionary = other.freq_dictionary;
        other.freq_dictionary = nullptr;
    }
    return *this;
}

void InvertedIndex::UpdateDocumentBase(std::vector<std::string> input_docs)
{
    docs = std::move(input_docs);
    if (docs.empty())
    {
        delete freq_dictionary;
        freq_dictionary = new std::map<std::string, std::vector<Entry>>();
        return;
    }

    std::vector<std::future<std::map<std::string, std::vector<Entry>>*>> index_future;
    std::vector<std::map<std::string, std::vector<Entry>>*> index_for_each_file;
    index_future.reserve(docs.size());
    index_for_each_file.reserve(docs.size());

    for (size_t i = 0; i < docs.size(); ++i)
    {
        index_future.push_back(std::async(std::launch::async, &InvertedIndex::SeparateIndexing,
                                          static_cast<int>(i), std::cref(docs[i])));
    }
    for (size_t i = 0; i < docs.size(); ++i)
    {
        index_for_each_file.push_back(index_future[i].get());
    }

    // Sort in descending order
    std::sort(begin(index_for_each_file), end(index_for_each_file),
              [](const auto* left, const auto* right) -> bool
              {
                  return left->size() > right->size();
              });

    const unsigned int hardware_threads = std::thread::hardware_concurrency() ? std::thread::hardware_concurrency() : 2;
    MergeAuxiliaryMaps(index_for_each_file, hardware_threads);

    // index_for_each_file[0] contents resulting map, delete others
    delete freq_dictionary;
    freq_dictionary = index_for_each_file[0];
    for (size_t i = 1; i < index_for_each_file.size(); ++i)
    {
        delete index_for_each_file[i];
    }
    index_for_each_file.clear();
}

std::map<std::string, std::vector<Entry>>* InvertedIndex::SeparateIndexing(int _docID, const std::string & txt_file_content)
{
    auto* separated_map = new std::map<std::string, std::vector<Entry>>();
    const char* start    = txt_file_content.c_str();
    const char* end      = start + txt_file_content.size();
    const char* iterator = start;
    while (iterator < end)
    {
        while (iterator < end && !std::isalpha(static_cast<unsigned char>(*iterator))) ++iterator;
        start = iterator;
        while (iterator < end && std::isalpha(static_cast<unsigned char>(*iterator))) ++iterator;
        if (start == iterator) continue;
        std::string word(start, iterator);
        std::transform(word.begin(), word.end(), word.begin(),
                       [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
        auto map_iterator = separated_map->find(word);
        if (map_iterator == separated_map->end())
        {
            separated_map->emplace(word, std::vector<Entry>{ Entry{ static_cast<size_t>(_docID), 1 } });
        }
        else
        {
            map_iterator->second[0].count++;
        }
    }
    return separated_map;
}

void InvertedIndex::MergeAuxiliaryMaps(std::vector<std::map<std::string, std::vector<Entry>>*>& auxiliary_maps,
                                       const unsigned int hardware_threads)
{
    if (auxiliary_maps.size() == 1) return;
    unsigned int threads_num = (auxiliary_maps.size() / 2) >= hardware_threads ? hardware_threads : (auxiliary_maps.size() / 2);
    std::vector<std::thread> threads;
    threads.reserve(threads_num);
    for (size_t i = 0; i < threads_num; ++i)
    {
        threads.emplace_back(MergeTwoSeparatedMaps, std::ref(auxiliary_maps), i, auxiliary_maps.size() - i - 1);
    }
    for (auto& it : threads)
    {
        if (it.joinable()) it.join();
    }
    for (unsigned int i = 0; i < threads_num; ++i)
    {
        delete auxiliary_maps.back();
        auxiliary_maps.pop_back();
    }
    if (auxiliary_maps.size() > 1)
        MergeAuxiliaryMaps(auxiliary_maps, hardware_threads);
}

void InvertedIndex::MergeTwoSeparatedMaps(std::vector<std::map<std::string, std::vector<Entry>>*>& sep_index_vec,
                                          size_t dst, size_t src)
{
    auto* dst_map = sep_index_vec[dst];
    auto* src_map = sep_index_vec[src];
    for (auto it = src_map->begin(); it != src_map->end();)
    {
        auto it_next = std::next(it);
        if (!dst_map->contains(it->first))
        {
            // Move node from src to dst
            auto node = src_map->extract(it);
            dst_map->insert(std::move(node));
        }
        else
        {
            MergeTwoSortedIndexVec((*dst_map)[it->first], (*src_map)[it->first]);
        }
        it = it_next;
    }
}

void InvertedIndex::MergeTwoSortedIndexVec(std::vector<Entry>& dst, std::vector<Entry>& src)
{
    if (src.empty()) return;
    std::vector<Entry> out;
    out.reserve(dst.size() + src.size());
    size_t i = 0, j = 0;
    while (i < dst.size() && j < src.size())
    {
        if (dst[i].docID < src[j].docID)
            out.push_back(std::move(dst[i++]));
        else if (dst[i].docID > src[j].docID)
            out.push_back(std::move(src[j++]));
        else
        {
            // Summarize counts for common docID
            dst[i].count += src[j].count;
            out.push_back(std::move(dst[i++]));
            ++j;
        }
    }
    while (i < dst.size()) out.push_back(std::move(dst[i++]));
    while (j < src.size()) out.push_back(std::move(src[j++]));
    dst = std::move(out);
    src.clear(); // delete src
}

std::vector<Entry> InvertedIndex::GetWordCount(const std::string &word) const
{
    if (!freq_dictionary) return {};
    auto it = freq_dictionary->find(word);
    if (it == freq_dictionary->end()) return {};
    return it->second;
}

// ----- SearchServer -----

std::vector<std::list<SearchTerm>> SearchServer::ParseRequest(const std::vector<std::string>& queries_input) const
{
    std::vector<std::list<SearchTerm>> parsed_requests(queries_input.size());
    for (size_t lineIndex = 0; lineIndex < queries_input.size(); ++lineIndex)
    {
        if (!queries_input[lineIndex].empty())
        {
            std::list<SearchTerm> lst;
            const char* start    = queries_input[lineIndex].c_str();
            const char* end      = start + queries_input[lineIndex].size();
            const char* iterator = start;
            while (iterator < end)
            {
                while (iterator < end && !std::isalpha(static_cast<unsigned char>(*iterator))) ++iterator;
                start = iterator;
                while (iterator < end && std::isalpha(static_cast<unsigned char>(*iterator))) ++iterator;
                if (start == iterator) continue;
                std::string word(start, iterator);
                std::transform(word.begin(), word.end(), word.begin(),
                               [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
                auto it = std::find_if(lst.begin(), lst.end(),
                                       [&](const SearchTerm& trm) { return trm.term == word; });
                if (it == lst.end())
                {
                    size_t cnt = 0;
                    auto entry_vec = _index->GetWordCount(word);
                    for (const auto& e : entry_vec) cnt += e.count;
                    if (cnt == 0) continue;
                    SearchTerm tmp{ word, cnt };
                    // Insertion with maintaining count's decreasing sorting
                    auto pos = std::find_if(lst.begin(), lst.end(),
                                            [&tmp](const SearchTerm& t) { return t.count < tmp.count; });
                    lst.insert(pos, tmp);
                }
            }
            parsed_requests[lineIndex] = std::move(lst);
        }
    }
    return parsed_requests;
}

size_t SearchServer::FindMaxAbsoluteRelevance(const std::list<AbsoluteIndex>& doc_list)
{
    size_t max = 0;
    for (const auto& doc : doc_list)
        if (doc.absolute_relevance > max) max = doc.absolute_relevance;
    return max;
}

std::vector<std::vector<RelativeIndex>> SearchServer::Search(const std::vector<std::string>& queries_input) const
{
    if (queries_input.empty())
    {
        std::cerr << "Warning: there is nothing to seek." << std::endl;
        return {};
    }

    auto parsed_requests = ParseRequest(queries_input);
    std::vector<std::list<AbsoluteIndex>> relevant_docs(parsed_requests.size());

    for (size_t request = 0; request < parsed_requests.size(); ++request)
    {
        if (parsed_requests[request].empty()) continue;

        const auto& first_term = parsed_requests[request].begin()->term;
        auto entry_vec = _index->GetWordCount(first_term);
        if (entry_vec.empty()) continue;

        std::list<AbsoluteIndex> relevant_docs_list;
        for (const auto& term_entry : entry_vec)
            relevant_docs_list.emplace_back(AbsoluteIndex{ term_entry.docID, term_entry.count });

        // Update relevance for other words
        for (auto it = std::next(parsed_requests[request].begin()); it != parsed_requests[request].end(); ++it)
        {
            auto entries = _index->GetWordCount(it->term);
            if (entries.empty()) continue;
            for (const auto& entry : entries)
            {
                auto doc_it = std::find_if(relevant_docs_list.begin(), relevant_docs_list.end(),
                                           [&entry](const AbsoluteIndex& ad) { return ad.docID == entry.docID; });
                if (doc_it != relevant_docs_list.end())
                    doc_it->absolute_relevance += entry.count;
            }
        }
        relevant_docs[request] = std::move(relevant_docs_list);
    }

    // Sort by decreasing absolute_relevance
    for (auto& lst : relevant_docs)
        lst.sort([](const AbsoluteIndex& a, const AbsoluteIndex& b) { return a.absolute_relevance > b.absolute_relevance; });

    std::vector<std::vector<RelativeIndex>> result(relevant_docs.size());
    for (size_t i = 0; i < relevant_docs.size(); ++i)
    {
        size_t max = FindMaxAbsoluteRelevance(relevant_docs[i]);
        if (max == 0) continue;
        for (const auto& doc : relevant_docs[i])
            result[i].emplace_back(RelativeIndex{ doc.docID, static_cast<float>(doc.absolute_relevance) / static_cast<float>(max) });
    }
    return result;
}
