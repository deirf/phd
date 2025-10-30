#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <cstdlib>
#include <cstring>
#include <string>
#include <algorithm>
#include <libconfig.h++>
#include "Configuration.h"
#include "Random.h"

using namespace libconfig;

std::string
GetDirectory(const std::string& _filename)
{
  std::string dirname;
  if (_filename.find('\\') != std::string::npos)
  {
    dirname = _filename.substr(0, _filename.find_last_of('\\'));
  }
  else
  {
    if (_filename.find('/') != std::string::npos)
      dirname = _filename.substr(0, _filename.find_last_of('/'));
    else
      dirname = ".";
  }
  return dirname;
}

std::string
ReplaceAll(std::string _str, const std::string& _from, const std::string& _to)
{
  size_t start_pos = 0;

  while ((start_pos = _str.find(_from, start_pos)) != std::string::npos)
  {
    _str.replace(start_pos, _from.length(), _to);
    start_pos += _to.length(); // Handles case where 'to' is a substring of 'from'
  }

  return _str;
}

Configuration::Configuration() {}

Configuration::~Configuration()
{
  if (ref_count == 1)
  {
    delete[] WorkPiece.K;
    delete[] WorkPiece.Cp;
    delete[] CoolingCurve.reference_function_values;
    delete[] CoolingCurve.reference_function_times;
  }

  this->ref_count--;
}

Configuration::Configuration(Configuration&& src)
{
  *this = std::move(src);
}

Configuration&
Configuration::operator=(Configuration&& src)
{
  *this = src;
  src.ref_count++;

  return *this;
}

ConfigurationLoader::ConfigurationLoader()
{
}

ConfigurationLoader::~ConfigurationLoader()
{
}

void
ConfigurationLoader::CalculateFixLengths(PREC _min, PREC _max, COUNT_T _N, PREC* _values)
{
  const PREC distance = ( _max - _min ) / ( (PREC)_N - 1 );
  for ( COUNT_T i = 0; i < _N; i++ )
  {
    _values[ i ] = (PREC)i * distance;
  }
}

PREC*
ConfigurationLoader::LoadArrayFromString(const std::string& _array, uint& _N, uint& _M)
{
  std::string clean = "";
  int occured = 3; // 0 - digit, 1 - white space, 2 - line break, 3 - footer
  for (unsigned int i = 0; i < _array.length(); i++)
  {
    if (_array[i] == '\t' || _array[i] == ' ' || _array[i] == ',')
    {
      occured = std::max(occured, 1);
    }

    if (_array[i] == '\n' || _array[i] == '|')
    {
      occured = std::max(occured, 2);
    }

    if (_array[i] == '.' || (_array[i] >= '0' && _array[i] <= '9')) // digit
    {
      if (occured == 2)
      {
        clean += '|';
      }
      else if (occured == 1)
      {
        clean += ' ';
      }
      clean += _array[i];
      occured = 0;
    }
  }

  // calculate dimensions
  _N = 1;
  for (unsigned int i = 0; i < clean.length(); i++)
  {
    if (clean[i] == '|') _N++;
  }

  _M = 0;
  for (unsigned int i = 0; i < clean.length(); i++)
  {
    if (clean[i] == ' ') _M++;
  }

  _M = _M / _N + 1;

  const uint element_count = (uint)( _N * _M + std::max( _N , _M ) );
  PREC* values = static_cast<PREC*>( _malloc( element_count * sizeof(PREC) ) );

  unsigned int i = 0;
  unsigned int j = 0;
  std::string word = "";
  while (i <= clean.length())
  {
    if (i != clean.length() && clean[i] != ' ' && clean[i] != '|')
    {
      word += clean[i];
    }
    else
    {
      PREC value = (PREC)stold(word);
      values[j++] = value;
      word = "";
    }
    i++;
  }

  return values;
}

std::string
ConfigurationLoader::ResolveTemplate(const std::string& _template, Configuration& _config)
{
  std::string result = ReplaceAll(_template, "{config}", _config.config_dir);
  result = ReplaceAll(result, "{mpath}", _config.Variables.measureDir);
  result = ReplaceAll(result, "{dim}", std::to_string(_config.Variables.searchingDimension) );

  return result;
}

PREC*
ConfigurationLoader::LoadArrayFromFile(const std::string& _filename, Configuration& _config, uint& _N, uint& _M)
{
  std::string FileName = ResolveTemplate(_filename, _config);
  std::ifstream fileread( FileName, std::ios::binary | std::ios::ate);
  ssize_t fsize = fileread.tellg();
  if (fsize < 0)
  {
    fileread.close();
    printf("Cannot open the %s file\n", FileName.c_str());
    DOES_NOT_REACH();
  }

  if (!fileread.good() || fsize == 0)
  {
    fileread.close();
    printf("%s file does not exists.\n", FileName.c_str());
    DOES_NOT_REACH();
  }

  std::string str = "";

  fileread.seekg(0, std::ios::end);
  str.reserve((size_t)fsize);
  fileread.seekg(0, std::ios::beg);

  str.assign( ( std::istreambuf_iterator<char>(fileread) ), std::istreambuf_iterator<char>() );

  fileread.close();

  return LoadArrayFromString(str, _N, _M);
}

Project
ConfigurationLoader::LoadProject(const std::string& filename, const std::string& config_string)
{
  Project result;
  UNUSED(filename);
  UNUSED(config_string);

  return result;
}

void ConfigurationLoader::LoadConfig_1D(Configuration* result, Config& cfg)
{
  const Setting& root = cfg.getRoot();
  std::string tempFileName;

  if (root.exists("CoolingCurve"))
  {
    const Setting& coolingcurve = root["CoolingCurve"];
    coolingcurve.lookupValue("InitialTemperature", result->CoolingCurve.initial_temperature);
    coolingcurve.lookupValue("FinalTemperature", result->CoolingCurve.final_temperature);

    coolingcurve.lookupValue("ReferenceFunction", tempFileName);
    result->CoolingCurve.ReferenceFunctionFileName = ResolveTemplate(tempFileName, *result);

    uint M;
    result->CoolingCurve.reference_function_N = 0;
    PREC* tempReferenceFunc = LoadArrayFromFile(result->CoolingCurve.ReferenceFunctionFileName, *result, result->CoolingCurve.reference_function_N, M);
    assume(result->CoolingCurve.reference_function_N > 0, "Reference function length is 0");

    result->CoolingCurve.reference_function_values = new PREC[result->CoolingCurve.reference_function_N]();
    result->CoolingCurve.reference_function_times = new PREC[result->CoolingCurve.reference_function_N]();
    for (uint i = 0; i < result->CoolingCurve.reference_function_N; i++)
    {
      result->CoolingCurve.reference_function_times[i] = tempReferenceFunc[i * 2];
      result->CoolingCurve.reference_function_values[i] = tempReferenceFunc[i * 2 + 1];
    }
    result->CoolingCurve.reference_function_end_time = tempReferenceFunc[(result->CoolingCurve.reference_function_N - 1) * 2];

    _free(tempReferenceFunc);
  }

  if (root.exists("WorkPiece"))
  {
    const Setting& workpiece = root["WorkPiece"];
    workpiece.lookupValue("R", result->WorkPiece.R);
    workpiece.lookupValue("N", result->WorkPiece.N);
    workpiece.lookupValue("TC", result->WorkPiece.TC);

    std::string shapefunction = "";
    workpiece.lookupValue("ShapeFunction", shapefunction);
    result->WorkPiece.shapeFunction = SF_UNKNOWN;
    if ( shapefunction == "HTC")
    {
      result->WorkPiece.shapeFunction = SF_HTC;
    }
    else if ( shapefunction == "Simple")
    {
      result->WorkPiece.shapeFunction = SF_SIMPLE;
    }
    else
    {
      assume(FALSE, "Unknown shapefunction: '%s'", shapefunction.c_str());
    }

    uint M;

    if (workpiece.exists("K"))
    {
      result->WorkPiece.K = LoadArrayFromString(workpiece.lookup("K"), result->WorkPiece.K_N, M);
    }

    if (workpiece.exists("Cp"))
    {
      result->WorkPiece.Cp = LoadArrayFromString(workpiece.lookup("Cp"), result->WorkPiece.Cp_N, M);
    }

    if (workpiece.exists("Rho"))
    {
      workpiece.lookupValue("Rho", result->WorkPiece.Rho);
    }
  }

  if (root.exists("TestFunctions"))
  {
    const Setting& TestFunctions = root["TestFunctions"];

    result->TestFunctions.reference_function_N = 1;
    if (TestFunctions.exists("ReferenceFunctionLen"))
    {
      TestFunctions.lookupValue("ReferenceFunctionLen", result->TestFunctions.reference_function_N);
    }

    result->TestFunctions.searching_dimension = 5;
    if (TestFunctions.exists("SearchingDimension"))
    {
      result->TestFunctions.searching_dimension = (unsigned int) std::stoul( ResolveTemplate( TestFunctions.lookup("SearchingDimension"), *result) );
    }

    result->TestFunctions.min_value = -100.0;
    if (TestFunctions.exists("MinValue"))
    {
      TestFunctions.lookupValue("MinValue", result->TestFunctions.min_value);
    }

    result->TestFunctions.max_value = 100.0;
    if (TestFunctions.exists("MaxValue"))
    {
      TestFunctions.lookupValue("MaxValue", result->TestFunctions.max_value);
    }
  }

  if (root.exists("IHCP"))
  {
    const Setting& ihcp = root["IHCP"];
    result->IHCP.min_T = -PREC_MAX;
    result->IHCP.max_T = PREC_MAX;
    if (ihcp.exists("MinT"))
      ihcp.lookupValue("MinT", result->IHCP.min_T);
    if (ihcp.exists("MaxT"))
      ihcp.lookupValue("MaxT", result->IHCP.max_T);

    result->IHCP.min_H = -PREC_MAX;
    result->IHCP.max_H = PREC_MAX;
    if (ihcp.exists("MinH"))
      ihcp.lookupValue("MinH", result->IHCP.min_H);
    if (ihcp.exists("MaxH"))
      ihcp.lookupValue("MaxH", result->IHCP.max_H);

    result->IHCP.min_dimension = 5;
    result->IHCP.max_dimension = 5;
    result->IHCP.searching_dimension = 5;
    if (ihcp.exists("MinDim"))
    {
      result->IHCP.min_dimension = (unsigned int) std::stoul( ResolveTemplate(ihcp.lookup("MinDim"), *result) );
    }
    if (ihcp.exists("MaxDim"))
    {
      result->IHCP.max_dimension = (unsigned int) std::stoul( ResolveTemplate(ihcp.lookup("MaxDim"), *result) );
    }
    if (ihcp.exists("SearchingDimension"))
    {
      result->IHCP.searching_dimension = (unsigned int) std::stoul( ResolveTemplate(ihcp.lookup("SearchingDimension"), *result) );
    }

    ihcp.lookupValue("HTCTimeFunction", tempFileName);
    result->HTCFunctionTimeFileName = ResolveTemplate(tempFileName, *result);

    uint M;
    uint N;
    result->IHCP.HTCTimeFunction = LoadArrayFromFile(ihcp.lookup("HTCTimeFunction"), *result, N, M);
    assume(N == result->IHCP.searching_dimension, "HTCTimeFunction's lenght is not as Searching dimension; N='%d', Dim='%d'", N, result->IHCP.searching_dimension);

    if (ihcp.exists("HTCInitValueFunction"))
    {
      ihcp.lookupValue("HTCInitValueFunction", tempFileName);
      result->HTCFunctionInitValueFileName = ResolveTemplate(tempFileName, *result);

      uint M;
      uint N;
      result->IHCP.HTCInitValueFunction = LoadArrayFromFile(ihcp.lookup("HTCInitValueFunction"), *result, N, M);
      assume(N == result->IHCP.searching_dimension, "HTCInitValueFunction's lenght is not as Searching dimension; N='%d', Dim='%d'", N, result->IHCP.searching_dimension);
    }
  }
}

Configuration
ConfigurationLoader::LoadConfig(const std::string& _filename, const std::string& config_string, bool _devFunction)
{
  Configuration result;
  result.config_filename = _filename;
  result.config_dir = GetDirectory(_filename);
  result.devFunction = _devFunction;

  std::string tempFilename;

  Config cfg;
  if (config_string == "")
  {
    cfg.readFile(_filename.c_str());
  }
  else
  {
    cfg.readString(config_string);
  }

  const Setting& root = cfg.getRoot();
  cfg.setAutoConvert(1);

  if (root.exists("Variables"))
  {
    const Setting& variables = root["Variables"];
    variables.lookupValue("SearchingDimension", result.Variables.searchingDimension);
    variables.lookupValue("MeasurePath", result.Variables.measureDir);
  }

  if (root.exists("Environment"))
  {
    const Setting& environment = root["Environment"];
    environment.lookupValue("CPU", result.Environment.CPU_N);
    environment.lookupValue("GPU", result.Environment.GPU_N);
    environment.lookupValue("Dimension", result.Environment.dimension);

    if (result.Environment.dimension == 1)
    {
      LoadConfig_1D(&result, cfg);
    }
  }

  //
  // PSO
  //

  if (root.exists("PSO"))
  {
    const Setting& PSO = root["PSO"];
    PSO.lookupValue("Size", result.PSO.size);

    if (PSO.exists("log_file"))
      PSO.lookupValue("log_file", tempFilename);
    else
      tempFilename = "swarms.log";
    result.PSO.Log_file = ResolveTemplate(tempFilename, result);

    if (PSO.exists("status_print_iter"))
      PSO.lookupValue("status_print_iter", result.PSO.status_print_iter);
    else
      result.PSO.status_print_iter = 10;

    if (PSO.exists("MaxIteration"))
      PSO.lookupValue("MaxIteration", result.PSO.max_iteration);
    else
      result.PSO.max_iteration = 0;
    if (PSO.exists("MinIteration"))
      PSO.lookupValue("MinIteration", result.PSO.min_iteration);
    else
      result.PSO.min_iteration = 0;

    if (PSO.exists("min_fitness"))
      PSO.lookupValue("min_fitness", result.PSO.min_fitness);
    else
      result.PSO.min_fitness = 1;
  }

  //
  // FWA
  //

  if (root.exists("nFWA"))
  {
    const Setting& nFWA = root["nFWA"];

    if (nFWA.exists("RandomSparkCount"))
      nFWA.lookupValue("RandomSparkCount", result.nFWA.RandomSparkCount);
    else
      result.nFWA.RandomSparkCount = 15;

    if (nFWA.exists("GaussianSparkCount"))
      nFWA.lookupValue("GaussianSparkCount", result.nFWA.GaussianSparkCount);
    else
      result.nFWA.GaussianSparkCount = 5;

    if (nFWA.exists("FireworkCount"))
      nFWA.lookupValue("FireworkCount", result.nFWA.FireworkCount);
    else
      result.nFWA.FireworkCount = 10;

    if (nFWA.exists("QuantumSparkCount"))
      nFWA.lookupValue("QuantumSparkCount", result.nFWA.QuantumSparkCount);
    else
      result.nFWA.QuantumSparkCount = 10;

    if (nFWA.exists("status_print_iter"))
      nFWA.lookupValue("status_print_iter", result.nFWA.status_print_iter);
    else
      result.nFWA.status_print_iter = 10;


    if (nFWA.exists("log_file"))
      nFWA.lookupValue("log_file", tempFilename);
    else
      tempFilename = "nFWA.log";
    result.nFWA.Log_file = ResolveTemplate(tempFilename, result);

    if (nFWA.exists("FireworkMaxIteration"))
      nFWA.lookupValue("FireworkMaxIteration", result.nFWA.FireworkMaxIteration);
    else
      result.nFWA.FireworkMaxIteration = 10;

    if (nFWA.exists("SparkMaxIteration"))
      nFWA.lookupValue("SparkMaxIteration", result.nFWA.SparkMaxIteration);
    else
      result.nFWA.SparkMaxIteration = 10;

    if (nFWA.exists("min_fitness"))
      nFWA.lookupValue("min_fitness", result.nFWA.min_fitness);
    else
      result.nFWA.min_fitness = 1;
  }


  //
  // oFWA
  //

  if (root.exists("oFWA"))
  {
    const Setting& oFWA = root["oFWA"];

    result.oFWA.max_firework_number = 50;
    if (oFWA.exists("max_firework_number"))
      oFWA.lookupValue("max_firework_number", result.oFWA.max_firework_number);

    result.oFWA.min_spark_number = 5;
    if (oFWA.exists("min_spark_number"))
      oFWA.lookupValue("min_spark_number", result.oFWA.min_spark_number);

    result.oFWA.max_spark_number = 50;
    if (oFWA.exists("max_spark_number"))
      oFWA.lookupValue("max_spark_number", result.oFWA.max_spark_number);

    result.oFWA.max_spark_iteration = 10;
    if (oFWA.exists("max_spark_iteration"))
      oFWA.lookupValue("max_spark_iteration", result.oFWA.max_spark_iteration);

    result.oFWA.max_firework_iteration = 1000;
    if (oFWA.exists("max_firework_iteration"))
      oFWA.lookupValue("max_firework_iteration", result.oFWA.max_firework_iteration);

    result.oFWA.min_fitness = 1;
    if (oFWA.exists("min_fitness"))
      oFWA.lookupValue("min_fitness", result.oFWA.min_fitness);

    result.oFWA.status_print_iter = 10;
    if (oFWA.exists("status_print_iter"))
      oFWA.lookupValue("status_print_iter", result.oFWA.status_print_iter);

    tempFilename = "oFWA.log";
    if (oFWA.exists("log_file"))
      oFWA.lookupValue("log_file", tempFilename);
    result.oFWA.log_file = ResolveTemplate(tempFilename, result);
  }

  result.ref_count++;
  return result;
}

ConfigurationTemplateHandler
ConfigurationLoader::LoadConfigTemplateHandler(const std::string& filename)
{
  ConfigurationTemplateHandler result;

  result.config_templ_filename = filename;
  result.config_templ_dir = GetDirectory(filename);
  result.config_file_filename = filename;

  std::ifstream t(result.config_file_filename);
  std::stringstream buffer;
  buffer << t.rdbuf();
  result.config_file_raw = buffer.str();

  return result;
}

Configuration
ConfigurationTemplateHandler::ApplyConfiguration(uint index, const uint _saindex, const uint _pfindex)
{
  std::string realization = config_file_raw;

  if (template_realizations.size() > 0) // no template
    for (auto vpair : template_realizations[index])
    {
      realization = ReplaceAll(realization, "\"{I:" + vpair.name + "}\"", vpair.value);
      realization = ReplaceAll(realization, "{" + vpair.name + "}", vpair.value);
    }

  realization = ReplaceAll(realization, "{SA}", solver_algsName[ _saindex ]);
  realization = ReplaceAll(realization, "{PF}", problem_funcsName[ _pfindex ]);

  return ConfigurationLoader::LoadConfig(config_file_filename, realization, _pfindex < PF_TECH_MAX);
}

Project
ConfigurationTemplateHandler::ApplyProject(uint index)
{
  std::string realization = config_file_raw;
  if (template_realizations.size() > 0) // no template
    for (auto vpair : template_realizations[index])
    {
      realization = ReplaceAll(realization, "\"{I:" + vpair.name + "}\"", vpair.value);
      realization = ReplaceAll(realization, "{" + vpair.name + "}", vpair.value);
    }

  return ConfigurationLoader::LoadProject(config_file_filename, realization);
}

//
// Context
//
Context::Context(const std::string& _template_filename,
                 const uint _saindex,
                 const uint _pfindex,
                 LogThread* _logthread,
                 ThreadPools* _threadpools,
                 uint _logLevel,
                 const std::string& _starttime,
                 uint _round
                )
{
  this->templater = ConfigurationLoader::LoadConfigTemplateHandler(_template_filename);

  this->project = templater.ApplyProject(0);
  this->config = templater.ApplyConfiguration(0, _saindex, _pfindex);
  this->logthread = _logthread;
  this->threadpools = _threadpools;
  this->sa_index = _saindex;
  this->pf_index = _pfindex;
  this->logLevel = _logLevel;
  this->starttime = _starttime;
  this->round = _round;
}
