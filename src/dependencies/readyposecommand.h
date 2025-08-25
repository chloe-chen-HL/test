#pragma once
#include "toreadyposecommand.h"

class ReadyPoseCommand : public Command {
public:
    ReadyPoseCommand();
    Type get_type() const override { return Type::STATUS_SET_READY_POSE; }
    std::string encode() const override;
    
    void set_pose(const std::vector<double>& pose);
    std::vector<double> get_pose() const;
    
private:
    std::vector<double> pose_;
};

using ReadyPoseCommandPtr = std::shared_ptr<ReadyPoseCommand>;