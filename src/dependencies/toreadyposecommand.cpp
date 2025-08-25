#include "toreadyposecommand.h"
#include <sstream>

ToReadyPoseCommand::ToReadyPoseCommand()
{
}

std::string ToReadyPoseCommand::encode() const
{
    std::ostringstream oss;
    oss << "ToReadyPoseCommand{pose:[";
    for (size_t i = 0; i < pose_.size(); ++i) {
        if (i > 0) oss << ",";
        oss << pose_[i];
    }
    oss << "],status:" << (status_ ? "true" : "false") << "}";
    return oss.str();
}

void ToReadyPoseCommand::set_pose(const std::vector<double>& pose)
{
    pose_ = pose;
}

std::vector<double> ToReadyPoseCommand::get_pose() const
{
    return pose_;
}

void ToReadyPoseCommand::set_status(bool status)
{
    status_ = status;
}

bool ToReadyPoseCommand::get_status() const
{
    return status_;
}

std::string Command::type2string(Type type)
{
    switch (type) {
        case Type::ACTION_MOVE_TO_READY:
            return "ACTION_MOVE_TO_READY";
        case Type::STATUS_SET_READY_POSE:
            return "STATUS_SET_READY_POSE";
        default:
            return "UNKNOWN";
    }
}