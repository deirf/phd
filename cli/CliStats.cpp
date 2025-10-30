#include "CliStats.h"

// ************************** static functions

static bool
runtimeCompare(CalculationResults* _a, CalculationResults* _b)
{
  return ( _a->ts < _b->ts );
}

static bool
fitnessCompare(CalculationResults* _a, CalculationResults* _b)
{
  return ( _a->fitness < _b->fitness );
}

static bool
calculationCompare(CalculationResults* _a, CalculationResults* _b)
{
  return ( _a->calculation < _b->calculation );
}

static bool
iterationCompare(CalculationResults* _a, CalculationResults* _b)
{
  return ( _a->iteration < _b->iteration );
}

// ************************** static functions

// ************************** calc functions

double
CliStats::calcMeanFitness(TResults* _results)
{
  const COUNT_T size = _results->size();
  double sum = 0;

  for ( COUNT_T i = 0 ; i < size; i++)
  {
    sum += (*_results)[i]->fitness;
  }

  return sum / (double)size;
}

double
CliStats::calcMedianFitness(TResults* _results)
{
  const COUNT_T size = _results->size();
  double median = 0;

  if ( size % 2 == 0 )
  {
    median = ( (*_results)[ size / 2 - 1 ]->fitness + (*_results)[ size / 2 ]->fitness ) / 2;
  }
  else
  {
    median = (*_results)[ size / 2 ]->fitness;
  }
  return median;
}

long int
CliStats::calcMeanRuntime(TResults* _results)
{
  const COUNT_T size = _results->size();
  long int sum = 0;

  for ( COUNT_T i = 0 ; i < size; i++)
  {
    sum += (*_results)[i]->ts;
  }

  return (long int)((double)sum / (double)size);
}

long int
CliStats::calcMedianRuntime(TResults* _results)
{
  const COUNT_T size = _results->size();
  long int median = 0;

  if ( size % 2 == 0 )
  {
    median = (long int) ( ( (*_results)[ size / 2 - 1 ]->ts + (*_results)[ size / 2 ]->ts ) / 2 );
  }
  else
  {
    median = (*_results)[ size / 2 ]->ts;
  }
  return median;
}

double
CliStats::calcStdDeviation(TResults* _results, double _mean)
{
  const COUNT_T size = _results->size();
  double stdDeviation = 0;

  for (COUNT_T i = 0; i < size; ++i)
  {
    stdDeviation += pow( (*_results)[ i ]->fitness - _mean, 2);
  }

  return sqrt( stdDeviation / (double)size );
}

double
CliStats::calcStdError(TResults* _results, double _stdDeviation)
{
  const double size = (double)_results->size();
  return _stdDeviation / sqrt( size );
}

// ************************** calc functions

// ************************** print functions

template<typename charT, typename traits>
static std::basic_ostream<charT, traits>&
operator<<(std::basic_ostream<charT, traits>& _s, const center_helper<charT, traits>& _c)
{
  _s << std::right;
  std::streamsize w = _s.width();
  if ( (long unsigned int)w > _c.str_.length() )
  {
    std::streamsize left = (long int)(((long unsigned int)w + _c.str_.length()) / 2);
    _s.width( left );
    _s << _c.str_;
    _s.width( w - left );
    _s << "";
  }
  else
  {
    _s << _c.str_;
  }
  return _s;
}

template<typename charT, typename traits>
static std::basic_ostream<charT, traits>&
operator<<(std::basic_ostream<charT, traits>& _s, const minMax_helper<charT, traits>& _c)
{
  double _min = 0;
  double _max = 0;

  switch (_c.type)
  {
    case CSMMH_DOUBLE:
    {
      _min = _c.dmin_;
      _max = _c.dmax_;
      break;
    }

    case CSMMH_LONG:
    {
      _min = (double)_c.lmin_;
      _max = (double)_c.lmax_;
      break;
    }

    case CSMMH_COUNT:
    {
      _min = (double)_c.cmin_;
      _max = (double)_c.cmax_;
      break;
    }

    default:
      DOES_NOT_REACH();
  }

  if ( _min == _max )
  {
    _s << std::left;
    _s << _min;
    return _s;
  }
  const long int p = _s.tellp();
  const std::streamsize w = _s.width();
  _s.width(2);
  _s << "L:";
  _s.width(0);
  _s << _min;
  _s.width(3);
  _s << " H:";
  _s.width(0);
  _s << _max;
  _s.width( p + w - _s.tellp() + 2 );

  return _s;
}


COUNT_T
CliStats::min_element_index( TResults* _vector, bool (*_function)(CalculationResults* _a, CalculationResults* _b) )
{
  const COUNT_T size = _vector->size();
  assume(size > 0, "Results element is 0");

  COUNT_T lowest = 0;
  if (size == 1)
  {
    return lowest;
  }

  for ( COUNT_T i = 0; i < size; i++)
  {
    if ( _function( (*_vector)[ i ], (*_vector)[ lowest ] ) )
    {
      lowest = i;
    }
  }

  return lowest;
}

COUNT_T
CliStats::max_element_index( TResults* _vector, bool (*_function)(CalculationResults* _a, CalculationResults* _b) )
{
  const COUNT_T size = _vector->size();
  assume(size > 0, "Results element is 0");

  COUNT_T highest = 0;
  if (size == 1)
  {
    return highest;
  }

  for ( COUNT_T i = 0; i < size; i++)
  {
    if ( _function( (*_vector)[ highest ], (*_vector)[ i ] ) )
    {
      highest = i;
    }
  }

  return highest;
}

std::string
CliStats::getMsgs( TResults* _vector )
{
  const COUNT_T size = _vector->size();
  assume(size > 0, "Results element is 0");

  std::string msg = "";

  for ( COUNT_T i = 0; i < size; i++)
  {
    const std::string new_msg = (*_vector)[ i ]->msg;
    if ( new_msg != "" )
    {
      if ( msg == "" )
      {
        msg = (*_vector)[ i ]->msg;
      }
      else
      {
        msg = msg + "; " + (*_vector)[ i ]->msg;
      }
    }
  }

  if ( msg == "" )
  {
    msg = "-";
  }

  return msg;
}

template<typename charT, typename traits = std::char_traits<charT> >
center_helper<charT, traits>
CliStats::centered(const std::basic_string<charT, traits>& _str)
{
  return center_helper<charT, traits>(_str);
}

center_helper<std::string::value_type, std::string::traits_type>
CliStats::centered(const std::string& _str)
{
  return center_helper<std::string::value_type, std::string::traits_type>(_str);
}

minMax_helper<std::string::value_type, std::string::traits_type>
CliStats::printMinMax(const double _min, const double _max)
{
  return minMax_helper<std::string::value_type, std::string::traits_type>(_min, _max);
}

minMax_helper<std::string::value_type, std::string::traits_type>
CliStats::printMinMax(const long _min, const long _max)
{
  return minMax_helper<std::string::value_type, std::string::traits_type>(_min, _max);
}

minMax_helper<std::string::value_type, std::string::traits_type>
CliStats::printMinMax(const COUNT_T _min, const COUNT_T _max)
{
  return minMax_helper<std::string::value_type, std::string::traits_type>(_min, _max);
}

// ************************** print functions

// ************************** result generation functions

std::string
CliStats::generateStats(TResults* _results, const std::string& _pfunc, const std::string& _sfunc)
{
  assume( (representable_as_type<COUNT_T, double>( _results->size() )), "Double is not representable as COUNT_T" );
  const COUNT_T size = _results->size();
  std::sort(_results->begin(), _results->end(), fitnessCompare);

  if ( size == 0 )
  {
    return "There is no any results.\n";
  }

  const double mean_fitness = this->calcMeanFitness(_results);
  const double median_fitness = this->calcMedianFitness(_results);
  const double stdDeviation = this->calcStdDeviation(_results, mean_fitness);
  const double stdError = this->calcStdError(_results, median_fitness);

  const COUNT_T min_calculation_index = this->min_element_index(_results, &calculationCompare);
  const COUNT_T max_calculation_index = this->max_element_index(_results, &calculationCompare);
  const COUNT_T min_calculation = (*_results)[ min_calculation_index ]->calculation;
  const COUNT_T max_calculation = (*_results)[ max_calculation_index ]->calculation;

  const COUNT_T min_iteration_index = this->min_element_index(_results, &iterationCompare);
  const COUNT_T max_iteration_index = this->max_element_index(_results, &iterationCompare);
  const COUNT_T min_iteration = (*_results)[ min_iteration_index ]->iteration;
  const COUNT_T max_iteration = (*_results)[ max_iteration_index ]->iteration;

  const COUNT_T min_fitness_index = this->min_element_index(_results, &fitnessCompare);
  const COUNT_T max_fitness_index = this->max_element_index(_results, &fitnessCompare);
  const double min_fitness = (*_results)[ min_fitness_index ]->fitness;
  const double max_fitness = (*_results)[ max_fitness_index ]->fitness;

  const COUNT_T min_shape_fitness_index = this->min_element_index(_results, &fitnessCompare);
  const COUNT_T max_shape_fitness_index = this->max_element_index(_results, &fitnessCompare);
  const double min_shape_fitness = (*_results)[ min_shape_fitness_index ]->shapefitness;
  const double max_shape_fitness = (*_results)[ max_shape_fitness_index ]->shapefitness;

  const COUNT_T min_runtime_index = this->min_element_index(_results, &runtimeCompare);
  const COUNT_T max_runtime_index = this->max_element_index(_results, &runtimeCompare);
  const double min_runtime = this->convertMicroSecToSec( (*_results)[ min_runtime_index ]->ts ) / THREADPOOL_SIZE;
  const double max_runtime = this->convertMicroSecToSec( (*_results)[ max_runtime_index ]->ts ) / THREADPOOL_SIZE;

  const double mean_runtime = this->convertMicroSecToSec( this->calcMeanRuntime(_results) ) / THREADPOOL_SIZE;
  const double median_runtime = this->convertMicroSecToSec( this->calcMedianRuntime(_results) ) / THREADPOOL_SIZE;

  const std::string msg = this->getMsgs( _results );
  const std::string space = std::string(PRINT_RESULT_SPACE_LEN, ' ');

  std::ostringstream print_data_str;
  print_data_str
    << std::setw(PRINT_RESULT_NAME_LEN)           << std::left << _pfunc + " (" + _sfunc + ")"             << space
    << std::setw(PRINT_RESULT_ROUND_LEN)          << centered( std::to_string(size) )                      << space
    << std::setw(PRINT_RESULT_CALC_LEN)           << printMinMax( min_calculation, max_calculation )       << space
    << std::setw(PRINT_RESULT_ITER_LEN)           << printMinMax( min_iteration, max_iteration )           << space
    << std::setw(PRINT_RESULT_MEANF_LEN)          << std::left << mean_fitness                             << space
    << std::setw(PRINT_RESULT_MEDIANF_LEN)        << std::left << median_fitness                           << space
    << std::setw(PRINT_RESULT_FITNESS_LEN)        << printMinMax( min_fitness, max_fitness )               << space
    << std::setw(PRINT_RESULT_SHAPEFITNESS_LEN)   << printMinMax( min_shape_fitness, max_shape_fitness )   << space
    << std::setw(PRINT_RESULT_STDDEV_LEN)         << std::left << stdDeviation                             << space
    << std::setw(PRINT_RESULT_STDERROR_LEN)       << std::left << stdError                                 << space
    << std::setw(PRINT_RESULT_RUNTIME_LEN)        << printMinMax( min_runtime, max_runtime )               << space
    << std::setw(PRINT_RESULT_MEANR_LEN)          << std::left << mean_runtime                             << space
    << std::setw(PRINT_RESULT_MEDIANR_LEN)        << std::left << median_runtime                           << space
    << std::setw(PRINT_RESULT_MSG_LEN)            << std::left << msg                                      << space
    << "\n";

  return print_data_str.str();
}

std::string
CliStats::generateNameAndRound(const std::string& _pfunc, const std::string& _sfunc, uint _round)
{
  std::ostringstream print_data_str;
  print_data_str
    << std::setw(PRINT_RESULT_NAME_LEN)      << std::left << _pfunc + " (" + _sfunc + ")" << std::string(PRINT_RESULT_SPACE_LEN, ' ')
    << std::setw(PRINT_RESULT_ROUND_LEN)     << centered( std::to_string(_round) )       << std::string(PRINT_RESULT_SPACE_LEN, ' ');

  return print_data_str.str();
}

// ************************** result generation functions

// ************************** public functions

void
CliStats::printStatsHeader()
{
  const std::string space = std::string(PRINT_RESULT_SPACE_LEN, ' ');

  std::cout
    << std::setw(PRINT_RESULT_NAME_LEN)           << std::left << PRINT_RESULT_NAME           << space
    << std::setw(PRINT_RESULT_ROUND_LEN)          << std::left << PRINT_RESULT_ROUND          << space
    << std::setw(PRINT_RESULT_CALC_LEN)           << std::left << PRINT_RESULT_CALC           << space
    << std::setw(PRINT_RESULT_ITER_LEN)           << std::left << PRINT_RESULT_ITER           << space
    << std::setw(PRINT_RESULT_MEANF_LEN)          << std::left << PRINT_RESULT_MEANF          << space
    << std::setw(PRINT_RESULT_MEDIANF_LEN)        << std::left << PRINT_RESULT_MEDIANF        << space
    << std::setw(PRINT_RESULT_FITNESS_LEN)        << std::left << PRINT_RESULT_FITNESS        << space
    << std::setw(PRINT_RESULT_SHAPEFITNESS_LEN)   << std::left << PRINT_RESULT_SHAPEFITNESS   << space
    << std::setw(PRINT_RESULT_STDDEV_LEN)         << std::left << PRINT_RESULT_STDDEV         << space
    << std::setw(PRINT_RESULT_STDERROR_LEN)       << std::left << PRINT_RESULT_STDERROR       << space
    << std::setw(PRINT_RESULT_RUNTIME_LEN)        << std::left << PRINT_RESULT_RUNTIME        << space
    << std::setw(PRINT_RESULT_MEANR_LEN)          << std::left << PRINT_RESULT_MEANR          << space
    << std::setw(PRINT_RESULT_MEDIANR_LEN)        << std::left << PRINT_RESULT_MEDIANR        << space
    << std::setw(PRINT_RESULT_MSG_LEN)            << std::left << PRINT_RESULT_MSG            << space
    << "\n";
}

void
CliStats::printNameAndRoundStats(const std::string& _pfunc, const std::string& _sfunc, uint _round)
{
    const std::string print_data_str = this->generateNameAndRound(_pfunc, _sfunc, _round);
    printf("\r%s\r", print_data_str.c_str());
}

void
CliStats::calulateAndPrintStats(TPFTestResults* _pfResults)
{
  PFTestResults* testresult = _pfResults->back();

  TResults* results = testresult->result;
  const std::string pfunc = testresult->pf_name;
  const std::string sfunc = testresult->sa_name;
  const std::string print_data_str = this->generateStats(results, pfunc, sfunc);

  printf("\r%s\r", print_data_str.c_str());

  delete results;
  _pfResults->pop_back();
}
