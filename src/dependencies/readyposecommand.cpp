#include "readyposecommand.h"
#include <sstream>

ReadyPoseCommand::ReadyPoseCommand()
{
}

std::string ReadyPoseCommand::encode() const
{
    std::ostringstream oss;
    oss << "ReadyPoseCommand{pose:[";
    for (size_t i = 0; i < pose_.size(); ++i) {
        if (i > 0) oss << ",";
        oss << pose_[i];
    }
    oss << "]}";
    return oss.str();
}

void ReadyPoseCommand::set_pose(const std::vector<double>& pose)
{
    pose_ = pose;
}

std::vector<double> ReadyPoseCommand::get_pose() const
{
    return pose_;
}