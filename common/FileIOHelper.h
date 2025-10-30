#pragma once
#include <string>
#include "Types.h"

using namespace std;

class FileIOHelper
{
public:
	static void LoadHTCFromFile(std::string filename, PREC* values, PREC& fitness);
	static bool isDirExist(const std::string& path);
	static bool makedir(const std::string& path);
	static std::string basename(const std::string& path);
	static std::string dirname(const std::string& path);
	static void copyFile(const std::string& _src, const std::string& _dst);
	static void copyConfig(const std::string& _config, const std::string& _dst);

	FileIOHelper() {}
	~FileIOHelper() {}
};

