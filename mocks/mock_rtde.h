#pragma once
#include <vector>
#include <memory>
#include <gmock/gmock.h>

// RTDE控制器接口
class RtdeInterface
{
public:
    virtual ~RtdeInterface() = default;
    virtual void movel(const std::vector<double>& pose) = 0;
    virtual void stopl() = 0;
    virtual int getAsyncProgress() = 0;
};

// RTDE控制器的模拟实现
class MockRtde : public RtdeInterface
{
public:
    MOCK_METHOD(void, movel, (const std::vector<double>& pose), (override));
    MOCK_METHOD(void, stopl, (), (override));
    MOCK_METHOD(int, getAsyncProgress, (), (override));
};

// 智能指针类型定义
using RtdePtr = std::shared_ptr<RtdeInterface>;