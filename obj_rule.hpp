#pragma once
#include "rule.hpp"

class ObjRule: public Rule
{
public:
    ObjRule(std::string);
protected:
    void internalResolve() override;
    std::string sourceFile_;
};
