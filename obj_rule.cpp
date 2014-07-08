#include "obj_rule.hpp"
#include "utils.hpp"
#include "file_rule.hpp"
#include "inc2lib.hpp"
#include <sstream>
#include <iostream>

ObjRule::ObjRule(std::string sourceFile):
    sourceFile_(sourceFile)
{
    target_ = "tmp/" + replace_extension(sourceFile, "o");
    dependencies_.push_back(std::unique_ptr<FileRule>(new FileRule(sourceFile)));
}

void ObjRule::internalResolve()
{
    {
        std::ostringstream cmd;
        cmd << "g++ -Wall -march=native -g -std=c++11 -c ";
        if (release)
            cmd << "-O3 ";
        cmd << sourceFile_;
        cmd << " -o " << target_;
        std::cout << cmd.str() << std::endl;
        auto res = system(cmd.str().c_str());
        if (res != 0)
            throw std::runtime_error("Compiling error");
    }
    {
        std::ostringstream cmd;
        cmd << "g++ -Wall -march=native -g -std=c++11 -MM -c ";
        if (release)
            cmd << "-O3 ";
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
        cmd << "g++ -Wall -march=native -g -std=c++11 -E -c ";
        if (release)
            cmd << "-O3 ";
        for (auto &d: dependencies_)
            if (extension(d->target()) == "cpp")
            {
                cmd << d->target();
                break;
            }
        cmd << " | grep ^#.*include | awk '{ print $3 }' | sort | uniq | sed 's/^.*\\///' | sed 's/\"//' > " << target_ << ".inc";
        auto res = system(cmd.str().c_str());
        if (res != 0)
            throw std::runtime_error("Compiling error");
    }
}
