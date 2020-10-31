#pragma once
#include "iterator_range.h"
#include "search_server.h"
#include "parse.h"
#include "test_runner.h"
#include "profile.h"
#include "duration.h"

#include <algorithm>
#include <iterator>
#include <map>
#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#include <random>
#include <thread>
#include <iomanip>
#include <ctime>
#include <chrono>
#include <string_view>
#include <exception>
#include <filesystem>

using namespace std;
using namespace chrono;
namespace fs = std::filesystem;

duration<int, milli> random_time(const size_t max_delay);
void TestParallelSearch(vector<pair<istream, ostream*>> streams);

void TestFunctionality(
  const vector<string>& docs,
  const vector<string>& queries,
  const vector<string>& expected
);
void TestSerpFormat();
void TestTop5();
void TestHitcount();
void TestRanking();
void TestBasicSearch();
void TestParallelSpeed();

struct Entry {
    size_t docid, hitcount;
  };

using Entries = map<size_t, size_t>;

class Vocabulary {
public:
  const vector<string>& Get() const;
protected:
  vector<string> voc;
  size_t voc_size;
};

class NaturalLangVocabulary: public Vocabulary {
public:
  NaturalLangVocabulary(const string& datafile);
};

class RandomLangVocabulary: public Vocabulary {
public:
  RandomLangVocabulary (const size_t vocabulary_size, const size_t word_length);
private:
  string random_string( size_t length );
};

template <typename Vocabulary>
istringstream GenerateRandomData(const Vocabulary& g, const size_t doc_count, const size_t words_in_doc) {
  ostringstream os;
  auto data = g.Get();
  size_t words_number = data.size();
  srand(time(NULL));
  for (size_t i = 0; i < doc_count; i++) {
    size_t first_ind = rand() % (words_number - words_in_doc);
    auto last_ind = first_ind + words_in_doc;
    for (auto ind = first_ind; ind < last_ind; ind++) {
      os << data[ind];
      if (ind + 1 < last_ind) {
        os << ' ';
      }
     }
     if (i + 1 < doc_count) {
       os << '\n';
     }
  }
  return istringstream(os.str());
};
