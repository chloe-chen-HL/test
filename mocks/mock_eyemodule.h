#pragma once
#include <memory>
#include <gmock/gmock.h>

// 视觉模块接口
class EyeModuleInterface
{
public:
    virtual ~EyeModuleInterface() = default;
    virtual bool isConnected() = 0;
    virtual void startCapture() = 0;
    virtual void stopCapture() = 0;
};

// 视觉模块的模拟实现
class MockEyeModule : public EyeModuleInterface
{
public:
    MOCK_METHOD(bool, isConnected, (), (override));
    MOCK_METHOD(void, startCapture, (), (override));
    MOCK_METHOD(void, stopCapture, (), (override));
};

// 智能指针类型定义
using EyeModulePtr = std::shared_ptr<EyeModuleInterface>;