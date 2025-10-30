#pragma once

#include <string>
#include "../common/Types.h"


template<typename charT, typename traits = std::char_traits<charT> >
class center_helper
{
  std::basic_string<charT, traits> str_ = "";

public:
  explicit center_helper(const std::basic_string<charT, traits>& _str) : str_(_str) {}

  template<typename a, typename b>
  friend std::basic_ostream<a, b>& operator<<(std::basic_ostream<a, b>& _s, const center_helper<a, b>& _c);
};


template<typename charT, typename traits = std::char_traits<charT> >
class minMax_helper
{
  double dmin_ = 0;
  double dmax_ = 0;
  long lmin_ = 0;
  long lmax_ = 0;
  COUNT_T cmin_ = 0;
  COUNT_T cmax_ = 0;

  COUNT_T type = 0;


public:
  minMax_helper(const double min, const double max) : dmin_(min), dmax_(max), type(CSMMH_DOUBLE) {}
  minMax_helper(const long min, const long max) : lmin_(min), lmax_(max), type(CSMMH_LONG) {}
  minMax_helper(const COUNT_T min, const COUNT_T max) : cmin_(min), cmax_(max), type(CSMMH_COUNT) {}

  template<typename a, typename b>
  friend std::basic_ostream<a, b>& operator<<(std::basic_ostream<a, b>& s, const minMax_helper<a, b>& c);
};
