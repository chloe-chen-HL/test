#pragma once
#include "statebase.h"
#include "toreadyposecommand.h"
#include "EyeModule.h"
#include "vector"
#include "sgnioRTDEControl.h"
#include "pedalcontroller.h"

class ToReadyState : public StateBase
{
public:
    ToReadyState(RtdePtr rtde, EyeModulePtr);
    virtual bool enter();
    virtual bool exit();

private:
    std::vector<std::string> urScript();
    void urProgress(int p);
    std::vector<double> getReadyPose();
    bool tooNear(std::vector<double>& p1,std::vector<double>& p2);

protected:
    virtual bool event(QEvent *e) override;
    virtual void clearContext() override;

private:
    RtdePtr rtde_ = nullptr;
    EyeModulePtr eye_ = nullptr;
    ToReadyPoseCommandPtr toReady_ = nullptr;
};