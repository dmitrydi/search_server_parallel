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

duration<int, std::milli> random_time(const size_t max_delay);
void TestParallelSearch(std::vector<std::pair<std::istream, std::ostream*>> streams);

void TestFunctionality(
  const std::vector<std::string>& docs,
  const std::vector<std::string>& queries,
  const std::vector<std::string>& expected
);
void TestSerpFormat();
void TestTop5();
void TestHitcount();
void TestRanking();
void TestBasicSearch();

