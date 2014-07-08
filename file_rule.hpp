#pragma once
#include "rule.hpp"
#include <string>

class FileRule: public Rule
{
public:
    FileRule(std::string);
protected:
    void internalResolve() override;
};
