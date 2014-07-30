// g++ *.cpp -o build -g -Wall -std=c++11
#include "bin_rule.hpp"
#include "config.hpp"
#include <iostream>
#include <stdexcept>

int main(int argc, char *argv[])
{
    readConfig();
    release = false;
    archive = false;
    for (int i = 1; i < argc; ++i)
        if (argv[i] == std::string("release"))
            release = true;
        else if (argv[i] == std::string("a"))
            archive = true;
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
