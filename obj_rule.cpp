#include "obj_rule.hpp"
#include "utils.hpp"
#include "file_rule.hpp"
#include "config.hpp"
#include <sstream>
#include <iostream>
#include <fstream>

ObjRule::ObjRule(std::string sourceFile):
    sourceFile_(sourceFile)
{
    target_ = "tmp/" + replace_extension(sourceFile, "o");
    dependencies_.push_back(std::unique_ptr<FileRule>(new FileRule(sourceFile)));
    if (exists(target_ + ".dep"))
    {
        std::ifstream f(target_ + ".dep");
        std::string s;
        f >> s;
        while (f.good())
        {
            f >> s;
            if (s != "\\")
                dependencies_.push_back(std::unique_ptr<FileRule>(new FileRule(s)));
        }
    }
}

void ObjRule::internalResolve()
{
    {
        std::ostringstream cmd;
        cmd << "g++ -Wall -march=native -gdwarf-3 -g -std=c++11 -c ";
        if (release)
            cmd << "-O3 ";
        if (exists(incDir))
            cmd << "-I" << incDir << " ";
        cmd << sourceFile_;
        cmd << " -o " << target_;
        std::cout << cmd.str() << std::endl;
        auto res = system(cmd.str().c_str());
        if (res != 0)
            throw std::runtime_error("Compiling error");
    }
    {
        std::ostringstream cmd;
        cmd << "g++ -Wall -march=native -gdwarf-3 -g -std=c++11 -MM -c ";
        if (release)
            cmd << "-O3 ";
        if (exists(incDir))
            cmd << "-I" << incDir << " ";
        for (auto &d: dependencies_)
            if (extension(d->target()) == "cpp")
            {
                cmd << d->target();
                break;
            }
        cmd << " -o " << target_ << ".dep";
        auto res = system(cmd.str().c_str());
        if (res != 0)
            throw std::runtime_error("Compiling error");
    }
    {
        std::ostringstream cmd;
        cmd << "g++ -Wall -march=native -gdwarf-3 -g -std=c++11 -E -c ";
        if (release)
            cmd << "-O3 ";
        if (exists(incDir))
            cmd << "-I" << incDir << " ";
        for (auto &d: dependencies_)
            if (extension(d->target()) == "cpp")
            {
                cmd << d->target();
                break;
            }
        cmd << " | grep ^# | awk '{ print $3 }' | sort | uniq | sed 's/^.*\\///' | sed 's/\"//g' > " << target_ << ".inc";
        auto res = system(cmd.str().c_str());
        if (res != 0)
            throw std::runtime_error("Compiling error");
    }
}
