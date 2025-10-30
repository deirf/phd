#pragma once

#include <string>
#include <vector>
#include "Types.h"
#include "LogThread.h"
#include "ThreadPool.h"
#include "Random.h"

std::string ReplaceAll(std::string _str, const std::string& _from, const std::string& _to);

class Project
{
public:
	std::string config_filename = "";
	std::string config_dir  = "";
};

class Configuration
{
friend class ConfigurationLoader;
public:
	std::string config_filename  = "";
	std::string config_dir  = "";
	int ref_count = 0;

	std::string HTCFunctionTimeFileName = "";
	std::string HTCFunctionValueFileName = "";
	std::string HTCFunctionInitValueFileName ="";
	PREC min_param = 0;
	PREC max_param = 0;
	uint searching_dimension = 0;
	uint reference_function_N = 0;
	bool devFunction = FALSE;

	struct {
		uint searchingDimension = 0;
		std::string measureDir = "";
	} Variables;

	struct {
		uint GPU_N = 0;
		uint CPU_N = 0;
		uint dimension = 0;
	} Environment;

	struct {
		PREC initial_temperature = 0;
		PREC final_temperature = 0;
		std::string ReferenceFunctionFileName = "";
		PREC* reference_function_values = nullptr;
		PREC* reference_function_times = nullptr;
		uint reference_function_N = 0;
		PREC reference_function_end_time = 0;
	} CoolingCurve;

	struct {
		PREC R = 0;
		PREC L = 0;
		uint TC = 0;
		uint N = 0;
		uint M = 0;
		uint K_N = 0;
		PREC* K = nullptr;
		uint Cp_N = 0;
		PREC* Cp = nullptr;
		PREC Rho = 0;
		COUNT_T shapeFunction = SF_UNKNOWN;
	} WorkPiece;

	struct {
		PREC min_T = 0;
		PREC max_T = 0;
		PREC min_H = 0;
		PREC max_H = 0;
		uint min_dimension = 0;
		uint max_dimension = 0;
		uint searching_dimension = 0;
		PREC *HTCTimeFunction = nullptr;
		PREC *HTCInitValueFunction = nullptr;
	} IHCP;

	struct {
		bool exists = FALSE;
		uint size = 0;

		std::string Log_file = "";
		uint max_iteration = 0;
		uint min_iteration = 0;
		uint status_print_iter = 10;
		PREC min_fitness = 1;
	} PSO;

	struct {
		bool exists = FALSE;
		uint searching_dimension = 0;
		PREC max_value = 0;
		PREC min_value = 0;
		uint reference_function_N = 0;
	} TestFunctions;

	struct {
		bool exists = FALSE;
		uint RandomSparkCount = 0;
		uint GaussianSparkCount = 0;
		uint FireworkCount = 0;
		uint QuantumSparkCount = 0;

		uint FireworkMaxIteration = 0;
		uint SparkMaxIteration = 0;

		std::string Log_file = "";
		uint status_print_iter = 10;
		PREC min_fitness = 1;
	} nFWA;

	struct {
		bool exists = FALSE;
  	uint min_spark_number = 0;
  	uint max_spark_number = 0;
  	uint max_spark_iteration = 0;
  	uint max_firework_iteration = 0;
  	uint max_firework_number = 0;
  	PREC min_fitness = 1;
		std::string log_file = "";
		uint status_print_iter = 10;
	} oFWA;



	Configuration();
	Configuration(Configuration&& src);
	~Configuration();
	Configuration &operator=(Configuration&& src);

private:
	Configuration &operator=(const Configuration& src) = default;
	Configuration(const Configuration& src) = delete;
};

class ConfigurationTemplateHandler
{
	friend class ConfigurationLoader;

	struct TemplateValue
	{
		std::string name;
		std::string value;
	};

	std::string config_templ_filename;
	std::string config_templ_dir;
	std::string config_file_filename;
	std::string config_file_raw;

	std::vector<std::vector<TemplateValue>> template_realizations;
public:
	Configuration ApplyConfiguration(uint index, const uint _saindex, const uint _pfindex);
	Project ApplyProject(uint index);
};

#include <libconfig.h++>
using namespace libconfig;

class ConfigurationLoader
{
private:
	static void LoadConfig_1D(Configuration* result, Config& cfg);
public:
	ConfigurationLoader();
	~ConfigurationLoader();

	static void CalculateFixLengths(PREC _min, PREC _max, COUNT_T _N, PREC* _values);
	static PREC* LoadArrayFromString(const std::string& _array, uint& _N, uint& _M);
	static PREC* LoadArrayFromFile(const std::string& _filename, Configuration& _config, uint& _N, uint& _M);
	static Configuration LoadConfig(const std::string& filename, const std::string& config_string = "", bool _devFunction = FALSE);
	static Project LoadProject(const std::string& filename, const std::string& config_string = "");
	static ConfigurationTemplateHandler LoadConfigTemplateHandler(const std::string& filename);
	static std::string ResolveTemplate(const std::string& _template, Configuration& _config);

};

class Context
{
	ConfigurationTemplateHandler templater;

public:
	Project project;
	Configuration config;
	LogThread* logthread = NULL;
	ThreadPools* threadpools = NULL;
	std::string starttime;
  UniformRnd<PREC> uniform_random;
  NormalRnd<PREC> normal_random;
  uint round = 0;
  uint sa_index = 0;
  uint pf_index = 0;
  uint logLevel = 0;

	Context(const std::string& _template_filename,
	        const uint _saindex,
	        const uint _pfindex,
	        LogThread* _logthread,
	        ThreadPools* _threadpools,
	        const uint _logLevel,
          const std::string& _starttime,
          uint _round
	       );
};
