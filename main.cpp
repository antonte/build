// g++ *.cpp -o build -g -Wall -std=c++11
#include "bin_rule.hpp"
#include "utils.hpp"
#include "inc2lib.hpp"
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

bool release = false;
std::map<std::string, std::vector<std::string>> inc2lib;

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
    
    try
    {
        BinRule target;
        target.resolve();
    }
    catch (std::runtime_error &e)
    {
        std::cerr << e.what() << std::endl;
        return 1;
    }
}
