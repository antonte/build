#include "file_rule.hpp"
#include <stdexcept>
#include <sstream>
#include <iostream>

FileRule::FileRule(std::string target)
{
    target_ = target;
}

void FileRule::internalResolve()
{
}
