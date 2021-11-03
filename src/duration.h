#pragma once
#include "profile.h"
#include <iostream>
#include <chrono>
#include <string>
#include <sstream>

using std::chrono::steady_clock;
using std::chrono::duration_cast;
using std::chrono::milliseconds;
using std::chrono::duration;

struct TotalDuration {
  std::string message;
  steady_clock::duration value;
  explicit TotalDuration(const std::string& msg = "")
        : message(msg + ": ")
        , value(0)
        {}

  ~TotalDuration() {
  std::ostringstream os;
  os << message
      << duration_cast<milliseconds>(value).count()
      << " ms" << std::endl;
  std::cerr << os.str();
  }
};

class AddDuration {
public:
  explicit AddDuration(steady_clock::duration& dest)
    : add_to(dest)
    , start(steady_clock::now())
    {}

  explicit AddDuration(TotalDuration& dest)
    : AddDuration(dest.value)
    {}

  ~AddDuration() {
      add_to += steady_clock::now() - start;
  }
private:
  steady_clock::duration& add_to;
  steady_clock::time_point start;
};

#define ADD_DURATION(value) \
AddDuration UNIQ_ID(__LINE__){value};
