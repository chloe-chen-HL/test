#pragma once
#include <vector>
#include <memory>

class sgnioRTDEControl {
public:
    virtual ~sgnioRTDEControl() = default;
    virtual void movel(const std::vector<double>& pose) = 0;
    virtual void stopl() = 0;
    virtual int getAsyncProgress() = 0;
};

using RtdePtr = std::shared_ptr<sgnioRTDEControl>;