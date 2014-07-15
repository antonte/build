#include "config.hpp"
#include "utils.hpp"
#include <unistd.h>
#include <fstream>
#include <sstream>
#include <memory>
#include <iostream>
#include <sys/types.h>
#include <dirent.h>

bool release = false;
std::map<std::string, std::vector<std::string>> inc2lib;
std::string incDir;
std::string libDir;
std::string cfgDir;
bool archive;

static void readConfig(std::string configFileName)
{
    std::ifstream cfg(configFileName);
    while (cfg.good())
    {
        std::string line;
        std::getline(cfg, line);
        if (!line.empty())
        {
            std::istringstream strm(line);
            std::string include;
            std::string library;
            strm >> include >> library;
            inc2lib[include].push_back(library);
        }
    }
}


void readConfig()
{
    auto exec = getExecutableName();
    auto execDir = exec.substr(0, exec.rfind("/"));
    auto configFileName = absolutePath(execDir + "/build.cfg");
    readConfig(configFileName);

    cfgDir = absolutePath(execDir + "/../a/build.d");
    std::unique_ptr<DIR, decltype(&closedir)> d(opendir(cfgDir.c_str()), &closedir);
    if (d)
        while (auto de = readdir(d.get()))
        {
            if (de->d_type == DT_REG && extension(de->d_name) == "cfg")
            {
                readConfig(cfgDir + "/" + de->d_name);
            }
        }

    incDir = absolutePath(execDir + "/../a/include");
    libDir = absolutePath(execDir + "/../a/lib");
}
