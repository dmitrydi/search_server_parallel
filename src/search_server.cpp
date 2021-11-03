#include "search_server.h"
#include "iterator_range.h"

#include <algorithm>
#include <iterator>
#include <sstream>
#include <iostream>
#include <future>

using namespace std;

vector<string> SplitIntoWords(string line) {
  istringstream words_input(move(line));
  return {
    make_move_iterator(istream_iterator<string>(words_input)),
    make_move_iterator(istream_iterator<string>())
  };
}

SearchServer::SearchServer(istream& document_input) {
    InvertedIndex new_index;
    for (string current_document; getline(document_input, current_document); ) {
        new_index.Add(move(current_document));
    }
    swap(index.GetAccess().ref_to_value, new_index);
}


void UpdateIndexSinglTread(istream& document_input, Synchronized<InvertedIndex>& index) {
    InvertedIndex new_index;
    for (string current_document; getline(document_input, current_document); ) {
        new_index.Add(move(current_document));
    }
    swap(index.GetAccess().ref_to_value, new_index);
}

void SearchServer::UpdateDocumentBase(istream& document_input) {
    futures.push_back(async( UpdateIndexSinglTread, ref(document_input), ref(index) ));
}

void AddQueriesStreamSingleThread(istream& query_input, ostream& search_results_output, Synchronized<InvertedIndex>& index) {
  size_t n = SearchServer::MAX_DOCS;
  vector<size_t> docid_count(n, 0);
  vector<pair<size_t, size_t>> search_results;

  for (string current_query; getline(query_input, current_query); ) {
    size_t max_id = 0;
    for (const auto& word: SplitIntoWords(current_query)) {
      for (auto [docid, count] : index.GetAccess().ref_to_value.Lookup(word)) {
        docid_count[docid] += count;
        if (docid > max_id) {
          max_id = docid;
        }
      }
    }
    search_results.clear();
    search_results.reserve(max_id+1);

    for (size_t i = 0; i <= max_id; i++) {
      if (docid_count[i] > 0) {
        search_results.push_back({i, docid_count[i]});
        docid_count[i] = 0;
      }
    }

    partial_sort(
      begin(search_results), begin(search_results) + min<size_t>(5, search_results.size()),
      end(search_results),
      [](pair<size_t, size_t> lhs, pair<size_t, size_t> rhs) {
        int64_t lhs_docid = lhs.first;
        auto lhs_hit_count = lhs.second;
        int64_t rhs_docid = rhs.first;
        auto rhs_hit_count = rhs.second;
        return make_pair(lhs_hit_count, -lhs_docid) > make_pair(rhs_hit_count, -rhs_docid);
      }
    );

    search_results_output << current_query << ':';
    for (auto [docid, hitcount] : Head(search_results, SearchServer::MAX_OUTPUT)) {
      if (hitcount > 0) {
        search_results_output << " {"
                              << "docid: " << docid << ", "
                              << "hitcount: " << hitcount << '}';
      }
    }
    search_results_output << endl;
  }
}

void SearchServer::AddQueriesStream(istream& query_input, ostream& search_results_output) {
  futures.push_back(async( AddQueriesStreamSingleThread, ref(query_input), ref(search_results_output), ref(index) ));
}

void InvertedIndex::Add(string &&document) {
  const size_t docid = size++;
  map<string, size_t> word_count;
  for (auto& word : SplitIntoWords(move(document))) {
    ++word_count[move(word)];
  }
  for (auto &[word, count] : word_count) {
    index[move(word)].push_back({docid, count});
  }
}

const List &InvertedIndex::Lookup(const string& word) const {
  if (auto it = index.find(word); it != index.end()) {
    return it->second;
  } else {
    return index_empty;
  }
}
