#include "file_rule.hpp"
#include <stdexcept>
#include <sstream>

FileRule::FileRule(std::string target)
{
    target_ = target;
}

void FileRule::internalResolve()
{
    std::ostringstream strm;
    strm << "The file '" << target_ << "' is missing";
    throw std::runtime_error(strm.str());
}
