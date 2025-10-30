#pragma once

#include <string>
#include <sstream>
#include <iomanip>
#include <iostream>

#include "CliStats_helper.h"
#include "../common/Types.h"


class CliStats
{
private:
  minMax_helper<std::string::value_type, std::string::traits_type> printMinMax(const double _min, const double _max);
  minMax_helper<std::string::value_type, std::string::traits_type> printMinMax(const long _min, const long _max);
  minMax_helper<std::string::value_type, std::string::traits_type> printMinMax(const COUNT_T _min, const COUNT_T _max);

  std::string generateNameAndRound(const std::string& _pfunc, const std::string& _sfunc, uint _round);
  std::string generateStats(TResults* _results, const std::string& _pfunc, const std::string& _sfunc);
  std::string getMsgs ( TResults* _vector );

  COUNT_T min_element_index ( TResults* _vector, bool (*_function)(CalculationResults* _a, CalculationResults* _b) );
  COUNT_T max_element_index ( TResults* _vector, bool (*_function)(CalculationResults* _a, CalculationResults* _b) );

  double convertMicroSecToSec( long usec) { return (double) usec / 1E6; }
  double calcMeanFitness(TResults* _results);
  double calcMedianFitness(TResults* _results);
  double calcStdDeviation(TResults* _results, double _mean);
  double calcStdError(TResults* _results, double _stdDeviation);

  long int calcMeanRuntime(TResults* _results);
  long int calcMedianRuntime(TResults* _results);

  template<typename charT, typename traits = std::char_traits<charT> >
    center_helper<charT, traits> centered(const std::basic_string<charT, traits>& _str);
  center_helper<std::string::value_type, std::string::traits_type> centered(const std::string& _str);

public:
  CliStats() {}
  ~CliStats() {}

  void printStatsHeader();
  void printNameAndRoundStats(const std::string& _pfunc, const std::string& _sfunc, uint _round);
  void calulateAndPrintStats(TPFTestResults* _pfResults);
};
