#pragma once
#include "mock_statebase.h"
#include "mock_command.h"
#include "mock_eyemodule.h"
#include "mock_rtde.h"
#include "mock_events.h"
#include <vector>
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

class ToReadyState : public StateBase
{
    Q_OBJECT

public:
    ToReadyState(RtdePtr rtde, EyeModulePtr eye);
    virtual bool enter() override;
    virtual bool exit() override;

private:
    std::vector<std::string> urScript();
    void urProgress(int p);
    std::vector<double> getReadyPose();
    bool tooNear(std::vector<double>& p1, std::vector<double>& p2);

protected:
    virtual bool event(QEvent *e) override;
    virtual void clearContext() override;

private:
    RtdePtr rtde_ = nullptr;
    EyeModulePtr eye_ = nullptr;
    ToReadyPoseCommandPtr toReady_ = nullptr;
};