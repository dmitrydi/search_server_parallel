#include "search_server.h"
#include "parse.h"
#include "test_runner.h"
#include "profile.h"
#include "duration.h"
#include "tests.h"
#include "iterator_range.h"

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

int main() {
  TestRunner tr;

  RUN_TEST(tr, TestSerpFormat);
  RUN_TEST(tr, TestTop5);
  RUN_TEST(tr, TestHitcount);
  RUN_TEST(tr, TestRanking);
  RUN_TEST(tr, TestBasicSearch);
}
