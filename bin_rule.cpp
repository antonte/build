#include "bin_rule.hpp"
#include "obj_rule.hpp"
#include "config.hpp"
#include "utils.hpp"
#include <memory>
#include <sstream>
#include <fstream>
#include <iostream>
#include <set>
#include <vector>
#include <stdexcept>
#include <sys/types.h>
#include <dirent.h>

BinRule::BinRule()
{
  create_directory("tmp");
  if (!archive)
    target_ = filename(current_path());
  else
    target_ = "lib" + filename(current_path()) + ".a";
  std::unique_ptr<DIR, decltype(&closedir)> d(opendir("."), &closedir);
  while (auto de = readdir(d.get()))
    if (de->d_type == DT_REG && extension(de->d_name) == "cpp")
      dependencies_.push_back(std::unique_ptr<ObjRule>(new ObjRule(de->d_name)));
}

void BinRule::internalResolve()
{
  std::vector<std::string> libs;
  if (!archive)
  {
    std::ostringstream cmd;
    cmd << "g++ -march=native -gdwarf-3 -pthread ";
    if (exists(libDir))
      cmd << "-L" << libDir << " ";
    for (auto &d: dependencies_)
    {
      cmd << d->target() << " ";
      std::ifstream f(d->target() + ".inc");
      while (f.good())
      {
        std::string s;
        f >> s;
        auto l = inc2lib[s];
        libs.insert(end(libs), begin(l), end(l));
      }
    }
    std::set<std::string> uniqueLibs;
    for (auto &i: libs)
      if (uniqueLibs.find(i) == std::end(uniqueLibs))
      {
        cmd << "-l" << i << " ";
        uniqueLibs.insert(i);
      }
    cmd << "-o " << target_;
    std::cout << cmd.str() << std::endl;
    auto res = system(cmd.str().c_str());
    if (res != 0)
      throw std::runtime_error("Linking error");
  }
  else
  {
    {
      std::ostringstream cmd;
      cmd << "ar r ";
      cmd << target_;
      std::set<std::string> uniqueLibs;
      for (auto &d: dependencies_)
      {
        cmd << " " << d->target();
        std::ifstream f(d->target() + ".inc");
        while (f.good())
        {
          std::string s;
          f >> s;
          for (auto lib: inc2lib[s])
            if (uniqueLibs.find(lib) == std::end(uniqueLibs))
            {
              libs.push_back(lib);
              uniqueLibs.insert(lib);
            }
        }
      }
      std::cout << cmd.str() << std::endl;
      auto res = system(cmd.str().c_str());
      if (res != 0)
        throw std::runtime_error("Linking error");
    }
    {
      std::ostringstream cmd;
      cmd << "mkdir -p " << incDir << "/a/";
      auto res = system(cmd.str().c_str());
      if (res != 0)
        throw std::runtime_error("Cannot create a directory ");
    }
    {
      std::ostringstream cmd;
      cmd << "mkdir -p " << libDir;
      auto res = system(cmd.str().c_str());
      if (res != 0)
        throw std::runtime_error("Cannot create a directory ");
    }
    {
      std::ostringstream cmd;
      cmd << "mkdir -p " << cfgDir;
      auto res = system(cmd.str().c_str());
      if (res != 0)
        throw std::runtime_error("Cannot create a directory ");
    }
        
    std::ofstream f(cfgDir + "/" + target_ + ".cfg");
    std::unique_ptr<DIR, decltype(&closedir)> d(opendir("."), &closedir);
    while (auto de = readdir(d.get()))
      if (de->d_type == DT_REG && extension(de->d_name) == "hpp")
      {
        std::ostringstream cmd;
        cmd << "ln -sf " << absolutePath(de->d_name) << " " << incDir << "/a/" << de->d_name;
        std::cout << cmd.str() << std::endl;
        auto res = system(cmd.str().c_str());
        if (res != 0)
          throw std::runtime_error("Cannot link a header file");
        auto tmp = filename(current_path());
        f << de->d_name << " " << tmp << std::endl;
        for (auto &i: libs)
          if (i != tmp)
            f << de->d_name << " " << i << std::endl;
      }
    std::ostringstream cmd;
    cmd << "ln -sf " << absolutePath(target_) << " " << libDir << "/" << target_;
    std::cout << cmd.str() << std::endl;
    auto res = system(cmd.str().c_str());
    if (res != 0)
      throw std::runtime_error("Cannot link a static library (archive) file");
  }
}
