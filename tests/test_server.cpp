#include "converter_json.hpp"
#include "search_server.hpp"
#include "gtest/gtest.h"

using namespace std;


void TestInvertedIndexFunctionality(const vector<string>& docs, const vector<string>& requests, const std::vector<vector<Entry>>& expected)
{
    std::vector<std::vector<Entry>> result;
    InvertedIndex idx;
    idx.UpdateDocumentBase(docs);
    for(auto& request: requests)
    {
        std::vector<Entry> word_count=idx.GetWordCount(request);
        result.push_back(word_count);
    }
    ASSERT_EQ(result, expected);
}


TEST(TestCaseInvertedIndex, TestBasic)
{
    const vector<string> docs = {"london is the capital of great britain", "bigben is the nickname for the Greatbell of the striking clock"};
    const vector<string> requests = {"london", "the"};
    const vector<vector<Entry>> expected ={{{0, 1}}, {{0, 1}, {1, 3}}};
    TestInvertedIndexFunctionality(docs, requests,expected);
}


TEST(TestCaseInvertedIndex, TestBasic2)
{
    const vector<string> docs = {"milk milk milk milk water water water", "milk water water", "milk milk milk milk milk water water water water water", "americano cappucchino"};
    const vector<string> requests = {"milk", "water", "cappucchino"};
    const vector<vector<Entry>> expected = {
        {{0, 4},{1, 1},{2, 5}},
        {{0, 3},{1, 2},{2, 5}},
        {{3, 1}}
    };
    TestInvertedIndexFunctionality(docs, requests,expected);
}


TEST(TestCaseInvertedIndex, TestInvertedIndexMissingWord)
{
    const vector<string> docs = {"abcdefghijkl", "statement"};
    const vector<string> requests = {"m", "statement"};
    const vector<vector<Entry>> expected = {{},{{1, 1}}};
    TestInvertedIndexFunctionality(docs, requests,expected);
}
