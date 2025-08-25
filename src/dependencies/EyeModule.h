#pragma once
#include <memory>

class EyeModule {
public:
    virtual ~EyeModule() = default;
    // 视觉模块相关方法
};

using EyeModulePtr = std::shared_ptr<EyeModule>;