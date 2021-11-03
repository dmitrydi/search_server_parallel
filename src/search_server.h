#pragma once

#include "syncronized.h"
#include <istream>
#include <ostream>
#include <set>
#include <list>
#include <vector>
#include <map>
#include <string>
#include <mutex>
#include <iostream>
#include <future>

using List = std::vector<std::pair<size_t, size_t> >;

class InvertedIndex {
public:
  void Add(std::string &&document);
  const List &Lookup(const std::string& word) const;

  size_t Size() const {
    return size;
  }

  std::map<std::string, List> GetIndex() {
    return index;
  }

private:
  std::map<std::string, List> index;
  List index_empty;
  size_t size = 0;
};

class SearchServer {
public:
  SearchServer() = default;
  explicit SearchServer(std::istream& document_input);
  void UpdateDocumentBase(std::istream& document_input);
  void AddQueriesStream(std::istream& query_input, std::ostream& search_results_output);
  static const size_t MAX_DOCS = 50000;
private:
  Synchronized<InvertedIndex> index;
  std::vector<std::future<void>> futures;
};
