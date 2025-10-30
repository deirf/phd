#include "FileIOHelper.h"
#include <iostream>
#include <fstream>
#include <string>
#include <sys/stat.h> // stat
#include <errno.h>    // errno, ENOENT, EEXIST

void
FileIOHelper::copyFile(const std::string& _src, const std::string& _dst)
{
    std::ifstream  src(_src, std::ios::binary);
    std::ofstream  dst(_dst, std::ios::binary);

    dst << src.rdbuf();
}

void
FileIOHelper::copyConfig(const std::string& _config, const std::string& _dst)
{
  const std::string path = FileIOHelper::dirname( _dst );
  const std::string basename = FileIOHelper::basename( _dst );
  FileIOHelper::copyFile(_config, path + "/" + basename + ".config");
}

void
FileIOHelper::LoadHTCFromFile(std::string filename, PREC* values, PREC& fitness)
{
	ifstream file;
	file.open(filename);
	std::string line;
	int i = 0;
	fitness = -1;
	while (!file.eof())
	{
		std::getline(file, line);
		if (line[0] == 'N')
		{
			line = line.substr(line.find(':')+1);
			fitness = std::stof(line);
			fitness *= fitness;
			std::getline(file, line);
			continue;
		}

		if (line.length() > 0)
		{
			PREC val = std::stof(line);
			values[i++] = val;
		}
	}
	file.close();
}

bool
FileIOHelper::isDirExist(const std::string& path)
{
    struct stat info;
    if (stat(path.c_str(), &info) != 0)
    {
			return false;
    }
    return (info.st_mode & S_IFDIR) != 0;
}

std::string
FileIOHelper::basename(const std::string& path)
{
  return path.substr(path.find_last_of("/\\") + 1);
}

std::string
FileIOHelper::dirname(const std::string& path)
{
  return path.substr(0, path.find_last_of("/\\"));
}

bool
FileIOHelper::makedir(const std::string& path)
{
    mode_t mode = 0755;
    int ret = mkdir(path.c_str(), mode);
    if (ret == 0)
    {
  		return TRUE;
    }

    switch (errno)
    {
	    case ENOENT:    // parent didn't exist, try to create it
			{
				size_t pos = path.find_last_of('/');
				if ( pos == std::string::npos )
				{
					return FALSE;
				}
				if ( ! makedir( path.substr(0, pos) ) )
				{
					return FALSE;
				}
			}
			// now, try to create again
			return 0 == mkdir(path.c_str(), mode);

	    case EEXIST:
				return TRUE; // isDirExist(path);

	    default:
				return FALSE;
    }

    DOES_NOT_REACH();
}
