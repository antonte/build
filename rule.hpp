#pragma once
#include <memory>
#include <string>
#include <vector>

class Rule
{
public:
    Rule();
    void resolve();
    std::string target() const;
protected:
    virtual void internalResolve() = 0;
    std::string target_;
    std::vector<std::unique_ptr<Rule> > dependencies_;
private:
    bool isResolved_;
};
