#include "config.hpp"
#include <unistd.h>
#include <sys/param.h>
#include <fstream>
#include <sstream>

bool release = false;
std::map<std::string, std::vector<std::string>> inc2lib;

static std::string absolutePath(std::string path)
{
  char * tmp = realpath(path.c_str(), NULL);
  if (tmp)
  {
    std::string res = tmp;
    free(tmp);
    return res;
  }
  else
    return path;
}

static std::string getExecutableName()
{
  char buf[MAXPATHLEN];
  ssize_t len;
  if ((len = readlink("/proc/self/exe", buf, sizeof(buf) - 1)) != -1)
    buf[len] = '\0';
  return buf;
}

static std::string getConfigFileName()
{
  auto exec = getExecutableName();
  auto execDir = exec.substr(0, exec.rfind("/"));
  return absolutePath(execDir + "/build.cfg");
}

void readConfig()
{
    std::ifstream cfg(getConfigFileName());
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
