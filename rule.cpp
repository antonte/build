#include "rule.hpp"
#include "utils.hpp"
#include <cassert>

Rule::Rule():
    isResolved_(false)
{}

std::string Rule::target() const
{
    return target_;
}

void Rule::resolve()
{
    if (isResolved_)
        return;
    bool outOfDate = !exists(target_);
    auto ts = outOfDate ? 0 : last_write_time(target_);
    for (auto &d: dependencies_)
    {
        d->resolve();
        outOfDate = outOfDate || !exists(d->target_) || (ts < last_write_time(d->target_));
    }
    if (outOfDate)
        internalResolve();
    isResolved_ = true;
}
