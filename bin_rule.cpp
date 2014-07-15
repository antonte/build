#include "bin_rule.hpp"
#include "obj_rule.hpp"
#include "config.hpp"
#include "utils.hpp"
#include <memory>
#include <sstream>
#include <set>
#include <fstream>
#include <iostream>
#include <sys/types.h>
#include <dirent.h>

BinRule::BinRule()
{
    create_directory("tmp");
    target_ = filename(current_path());
    std::unique_ptr<DIR, decltype(&closedir)> d(opendir("."), &closedir);
    while (auto de = readdir(d.get()))
        if (de->d_type == DT_REG && extension(de->d_name) == "cpp")
            dependencies_.push_back(std::unique_ptr<ObjRule>(new ObjRule(de->d_name)));
}

void BinRule::internalResolve()
{
    std::ostringstream cmd;
    cmd << "g++ -march=native ";
    std::set<std::string> libs;
    for (auto &d: dependencies_)
    {
        cmd << d->target() << " ";
        std::ifstream f(d->target() + ".inc");
        while (f.good())
        {
            std::string s;
            f >> s;
            auto l = inc2lib[s];
            libs.insert(begin(l), end(l));
        }
    }
    for (auto &i: libs)
        cmd << "-l" << i << " ";
    cmd << "-o " << target_;
    std::cout << cmd.str() << std::endl;
    auto res = system(cmd.str().c_str());
    if (res != 0)
        throw std::runtime_error("Linking error");
}
