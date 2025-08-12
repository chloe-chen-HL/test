#pragma once
#include <string>
#include "needlecontroller.h"

template <typename E>
class EnumRegistry {
public:
    static EnumRegistry& instance() {
        static EnumRegistry inst;
        return inst;
    }

    E fromString(const std::string& name) const;
};

template <>
inline NeedleController::LightColor EnumRegistry<NeedleController::LightColor>::fromString(const std::string& name) const {
    if (name == "Red" || name == "red") return NeedleController::LightColor::Red;
    if (name == "Green" || name == "green") return NeedleController::LightColor::Green;
    if (name == "Blue" || name == "blue") return NeedleController::LightColor::Blue;
    return NeedleController::LightColor::Red;
}