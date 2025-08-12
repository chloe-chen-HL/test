#pragma once
#include <memory>
#include "needlecontroller.h"

class StateMachine {
public:
    std::shared_ptr<NeedleController> get_needle() const { return needle_; }
    void set_needle(std::shared_ptr<NeedleController> needle) { needle_ = std::move(needle); }

private:
    std::shared_ptr<NeedleController> needle_;
};