// g++ main.cpp -o build -g -Wall -lboost_system -lboost_filesystem -std=c++0x
#include <cstdlib>
#include <iostream>
#include <string>
#include <boost/filesystem.hpp>
#include <map>
#include <sstream>
#include <exception>
#include <fstream>
#include <set>

namespace fs = boost::filesystem;

bool release = false;

struct Rule
{
    enum ResolvingType { Link, Compile, MkDir };
    Rule(): isResolved(false) {}
    std::vector<fs::path> dependencies;
    ResolvingType resolvingType;
    bool isResolved;
};

std::map<fs::path, Rule> dependencies;

std::map<std::string, std::vector<std::string>> inc2lib;

void build(fs::path target)
{
    auto ids = dependencies.find(target);
    if (ids == dependencies.end())
    {
        if (fs::exists(target))
            return;
        else
            throw std::runtime_error("There is not rule for " + target.native());
    }
    auto ds = ids->second; // make hard copy
    if (ds.isResolved)
        return;
    bool outOfDate = !fs::exists(target);
    auto ts = outOfDate ? 0 : fs::last_write_time(target);
    auto dependencies = ds.dependencies; // make hard copy
    for (auto &d: dependencies)
    {
        build(d);
        if (!fs::is_directory(d))
            outOfDate = outOfDate || (ts < fs::last_write_time(d));
    }
    if (outOfDate)
    {
        switch (ds.resolvingType)
        {
        case Rule::Link:
            {
                std::ostringstream cmd;
                cmd << "g++ ";
                std::set<std::string> libs;
                for (auto &d: ds.dependencies)
                {
                    cmd << d << " ";
                    std::ifstream f(d.native() + ".inc");
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
                cmd << "-o " << target;
                std::cout << cmd.str() << std::endl;
                auto res = system(cmd.str().c_str());
                if (res != 0)
                    throw std::runtime_error("Linking error");
            }
            break;
        case Rule::Compile:
            {
                {
                    std::ostringstream cmd;
                    cmd << "g++ -Wall -g -std=c++0x -c ";
                    if (release)
                        cmd << "-O3 ";
                    for (auto &d: ds.dependencies)
                        if (d.extension() == ".cpp")
                        {
                            cmd << d;
                            break;
                        }
                    cmd << " -o " << target;
                    std::cout << cmd.str() << std::endl;
                    auto res = system(cmd.str().c_str());
                    if (res != 0)
                        throw std::runtime_error("Compiling error");
                }
                {
                    std::ostringstream cmd;
                    cmd << "g++ -Wall -g -std=c++0x -MM -c ";
                    if (release)
                        cmd << "-O3 ";
                    for (auto &d: ds.dependencies)
                        if (d.extension() == ".cpp")
                        {
                            cmd << d;
                            break;
                        }
                    cmd << " -o " << target << ".dep";
                    auto res = system(cmd.str().c_str());
                    if (res != 0)
                        throw std::runtime_error("Compiling error");
                }
                {
                    std::ostringstream cmd;
                    cmd << "g++ -Wall -g -std=c++0x -E -c ";
                    if (release)
                        cmd << "-O3 ";
                    for (auto &d: ds.dependencies)
                        if (d.extension() == ".cpp")
                        {
                            cmd << d;
                            break;
                        }
                    cmd << " | grep ^#.*include | awk '{ print $3 }' | sort | uniq | sed 's/^.*\\///' | sed 's/\"//' > " << target << ".inc";
                    auto res = system(cmd.str().c_str());
                    if (res != 0)
                        throw std::runtime_error("Compiling error");
                }
            }
            break;
        case Rule::MkDir:
            fs::create_directory(target);
            break;
        }
    }
    assert(fs::exists(target));
    ds.isResolved = true;
}

int main(int argc, char *argv[])
{
    release = (argc == 2 && argv[1] == std::string("release"));
    inc2lib["glut.h"].push_back("glut");
    inc2lib["glut.h"].push_back("GL");
    inc2lib["gl.h"].push_back("GL");
    inc2lib["fcgio.h"].push_back("fcgi++");
    inc2lib["fcgio.h"].push_back("fcgi");
    inc2lib["asio.hpp"].push_back("boost_system");
    inc2lib["asio.hpp"].push_back("pthread");
    inc2lib["filesystem.hpp"].push_back("boost_system");
    inc2lib["filesystem.hpp"].push_back("boost_filesystem");
    inc2lib["regex.hpp"].push_back("boost_system");
    inc2lib["regex.hpp"].push_back("boost_regex");
    inc2lib["curl.h"].push_back("curl");
    inc2lib["json_spirit.h"].push_back("json_spirit");
    inc2lib["json.h"].push_back("json");
    inc2lib["speex_preprocess.h"].push_back("m");
    inc2lib["speex_preprocess.h"].push_back("speexdsp");
    inc2lib["gmpxx.h"].push_back("gmpxx");
    inc2lib["gmpxx.h"].push_back("gmp");
    inc2lib["simple.h"].push_back("pulse-simple");
    inc2lib["hiredis.h"].push_back("hiredis");
    inc2lib["png.h"].push_back("png");
    inc2lib["text_oarchive.hpp"].push_back("boost_serialization-mt");
    inc2lib["text_iarchive.hpp"].push_back("boost_serialization-mt");
    inc2lib["text_woarchive.hpp"].push_back("boost_serialization-mt");
    inc2lib["text_wiarchive.hpp"].push_back("boost_serialization-mt");
    inc2lib["xml_oarchive.hpp"].push_back("boost_serialization-mt");
    inc2lib["xml_iarchive.hpp"].push_back("boost_serialization-mt");
    inc2lib["xml_woarchive.hpp"].push_back("boost_serialization-mt");
    inc2lib["xml_wiarchive.hpp"].push_back("boost_serialization-mt");
    inc2lib["binary_oarchive.hpp"].push_back("boost_serialization-mt");
    inc2lib["binary_iarchive.hpp"].push_back("boost_serialization-mt");
    inc2lib["avformat.h"].push_back("avformat");
    inc2lib["avdevice.h"].push_back("avdevice");
    inc2lib["rc4.h"].push_back("crypto");
    inc2lib["evp.h"].push_back("ssl");
    inc2lib["evp.h"].push_back("crypto");
    inc2lib["tinyxml.h"].push_back("tinyxml");
    inc2lib["thread"].push_back("pthread");
    inc2lib["swscale.h"].push_back("swscale");
    inc2lib["fftw3.h"].push_back("fftw3");
    inc2lib["fftw3.h"].push_back("m");
    inc2lib["zmq.h"].push_back("zmq");
    inc2lib["zmq.hpp"].push_back("zmq");
    inc2lib["amqp.h"].push_back("rabbitmq");
    inc2lib["amqp_framing.h"].push_back("rabbitmq");
    inc2lib["SDL.h"].push_back("SDL2");
    
    auto dir = fs::current_path();
    auto target = dir.filename();
    if (fs::exists(dir) && fs::is_directory(dir))
    {
        fs::directory_iterator endIter;
        for (fs::directory_iterator i(dir); i != endIter; ++i)
            if (fs::is_regular_file(i->status()))
            {
                auto ext = i->path().extension();
                auto path = i->path();
                if (path.extension() == ".cpp")
                {
                    auto obj = fs::path("tmp") / (path.replace_extension().filename().native() +  ".o");
                    dependencies[target].dependencies.push_back(obj);
                    dependencies[target].resolvingType = Rule::Link;
                    dependencies[obj].dependencies.push_back("tmp");
                    dependencies["tmp"].resolvingType = Rule::MkDir;
                    dependencies[obj].resolvingType = Rule::Compile;
                    if (fs::exists(obj.native() + ".dep"))
                    {
                        std::ifstream f(obj.native() + ".dep");
                        std::string s;
                        f >> s;
                        while (f.good())
                        {
                            f >> s;
                            dependencies[obj].dependencies.push_back(s);
                        }
                    }
                    else
                        dependencies[obj].dependencies.push_back(i->path().filename());
                }
            }
    }
    try
    {
        build(target);
    }
    catch (std::runtime_error &)
    {
        return 1;
    }
}
