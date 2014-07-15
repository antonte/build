// g++ *.cpp -o build -g -Wall -std=c++11
#include "bin_rule.hpp"
#include "config.hpp"
#include <iostream>

int main(int argc, char *argv[])
{
    readConfig();
    release = (argc == 2 && argv[1] == std::string("release"));
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
