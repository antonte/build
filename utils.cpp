#include "utils.hpp"
#include <fstream>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/param.h>
#ifdef __APPLE__
#include <mach-o/dyld.h>
#endif

bool exists(std::string fileName)
{
    return std::ifstream(fileName).good();
}

time_t last_write_time(std::string fileName)
{
  struct stat buffer;   
  if (stat(fileName.c_str(), &buffer) != 0)
      return 0;
  return buffer.st_mtime;
}

bool is_directory(std::string fileName)
{
    struct stat buffer;   
    if (stat(fileName.c_str(), &buffer) != 0)
        return false;
    return buffer.st_mode & S_IFDIR;
}

std::string extension(std::string fileName)
{
    auto idx = fileName.rfind('.');
    return idx != std::string::npos ? fileName.substr(idx + 1) : "";
}

void create_directory(std::string dir)
{
    mkdir(dir.c_str(), 0777);
}

std::string current_path()
{
    std::string res;
    char *cres = getcwd(nullptr, 0);
    res = cres;
    free(cres);
    return res;
}

std::string filename(std::string fileName)
{
    auto idx = fileName.rfind('/');
    return idx != std::string::npos ? fileName.substr(idx + 1) : "";
}

std::string replace_extension(std::string path, std::string ext)
{
    return path + "." + ext;
}

std::string absolutePath(std::string path)
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

#ifndef __APPLE__
std::string getExecutableName()
{
    char buf[MAXPATHLEN];
    ssize_t len;
    if ((len = readlink("/proc/self/exe", buf, sizeof(buf) - 1)) != -1)
        buf[len] = '\0';
    return buf;
}
#else
std::string getExecutableName()
{
    char exe[MAXPATHLEN];
    uint32_t bufSize = sizeof(exe);
    _NSGetExecutablePath(exe, &bufSize);
    char buf[MAXPATHLEN];
    ssize_t len;
    if ((len = readlink(exe, buf, sizeof(buf) - 1)) != -1)
        buf[len] = '\0';
    return buf;
}
#endif
