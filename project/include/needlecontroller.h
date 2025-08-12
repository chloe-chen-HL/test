#pragma once
#include <optional>

struct NeedleController {
    enum class LightColor { Red, Green, Blue };

    void setLightColor(LightColor color) {
        lastColor = color;
        called = true;
    }

    bool called { false };
    std::optional<LightColor> lastColor;
};