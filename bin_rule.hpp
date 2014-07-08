#pragma once
#include "rule.hpp"

class BinRule: public Rule
{
public:
    BinRule();
protected:
    void internalResolve() override;
    const std::string binName_;
};
