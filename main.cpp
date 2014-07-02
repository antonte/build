// g++ main.cpp -o build -g -Wall -std=c++0x
#include <cstdlib>
#include <iostream>
#include <string>
#include <map>
#include <sstream>
#include <exception>
#include <fstream>
#include <set>
#include <vector>
#include <stdexcept>
#include <cassert>
#include <memory>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>

bool release = false;

struct Rule
{
    enum ResolvingType { Link, Compile, MkDir };
    Rule(): isResolved(false) {}
    std::vector<std::string> dependencies;
    ResolvingType resolvingType;
    bool isResolved;
};

std::map<std::string, Rule> dependencies;

std::map<std::string, std::vector<std::string>> inc2lib;

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

void build(std::string target)
{
    auto ids = dependencies.find(target);
    if (ids == dependencies.end())
    {
        if (exists(target))
            return;
        else
            throw std::runtime_error("There is not rule for " + target);
    }
    auto ds = ids->second; // make hard copy
    if (ds.isResolved)
        return;
    bool outOfDate = !exists(target);
    auto ts = outOfDate ? 0 : last_write_time(target);
    auto dependencies = ds.dependencies; // make hard copy
    for (auto &d: dependencies)
    {
        build(d);
        if (!is_directory(d))
            outOfDate = outOfDate || (ts < last_write_time(d));
    }
    if (outOfDate)
    {
        switch (ds.resolvingType)
        {
        case Rule::Link:
            {
                std::ostringstream cmd;
                cmd << "g++ -march=native ";
                std::set<std::string> libs;
                for (auto &d: ds.dependencies)
                {
                    cmd << d << " ";
                    std::ifstream f(d + ".inc");
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
                    cmd << "g++ -Wall -march=native -g -std=c++0x -c ";
                    if (release)
                        cmd << "-O3 ";
                    for (auto &d: ds.dependencies)
                        if (extension(d) == "cpp")
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
                    cmd << "g++ -Wall -march=native -g -std=c++0x -MM -c ";
                    if (release)
                        cmd << "-O3 ";
                    for (auto &d: ds.dependencies)
                        if (extension(d) == "cpp")
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
                    cmd << "g++ -Wall -march=native -g -std=c++0x -E -c ";
                    if (release)
                        cmd << "-O3 ";
                    for (auto &d: ds.dependencies)
                        if (extension(d) == "cpp")
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
            create_directory(target);
            break;
        }
    }
    assert(exists(target));
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
    inc2lib["SDL_ttf.h"].push_back("SDL2_ttf");
    inc2lib["Xlib.h"].push_back("X11");
    inc2lib["Xlib.h"].push_back("X11");
    inc2lib["Xutil.h"].push_back("X11");
    inc2lib["Xos.h"].push_back("X11");
    
    auto dir = current_path();
    auto target = filename(dir);
    if (exists(dir) && is_directory(dir))
    {
        std::unique_ptr<DIR, decltype(&closedir)> d(opendir(dir.c_str()), &closedir);
        while (auto de = readdir(d.get()))
        {
            if (de->d_type == DT_REG)
            {
                auto ext = extension(de->d_name);
                auto path = de->d_name;
                if (ext == "cpp")
                {
                    auto obj = "tmp/" + replace_extension(path, "o");
                    dependencies[target].dependencies.push_back(obj);
                    dependencies[target].resolvingType = Rule::Link;
                    dependencies[obj].dependencies.push_back("tmp");
                    dependencies["tmp"].resolvingType = Rule::MkDir;
                    dependencies[obj].resolvingType = Rule::Compile;
                    if (exists(obj + ".dep"))
                    {
                        std::ifstream f(obj + ".dep");
                        std::string s;
                        f >> s;
                        while (f.good())
                        {
                            f >> s;
                            if (s != "\\")
                            dependencies[obj].dependencies.push_back(s);
                        }
                    }
                    else
                        dependencies[obj].dependencies.push_back(path);
                }
            }
        }
    }
    try
    {
        build(target);
    }
    catch (std::runtime_error &e)
    {
        std::cerr << e.what() << std::endl;
        return 1;
    }
}
