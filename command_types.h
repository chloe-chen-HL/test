#ifndef COMMAND_TYPES_H
#define COMMAND_TYPES_H

#include <string>

enum class CommandType {
    ACTION_MOVE_TO_READY,
    OTHER_COMMAND
};

class Command {
public:
    virtual ~Command() = default;
    virtual CommandType get_type() const = 0;
    
    static std::string type2string(CommandType type) {
        switch (type) {
            case CommandType::ACTION_MOVE_TO_READY:
                return "ACTION_MOVE_TO_READY";
            case CommandType::OTHER_COMMAND:
                return "OTHER_COMMAND";
            default:
                return "UNKNOWN";
        }
    }
};

struct Pose {
    double x, y, z;
    double roll, pitch, yaw;
    
    bool operator==(const Pose& other) const {
        return x == other.x && y == other.y && z == other.z &&
               roll == other.roll && pitch == other.pitch && yaw == other.yaw;
    }
};

class ToReadyPoseCommand : public Command {
private:
    Pose pose_;
    
public:
    CommandType get_type() const override {
        return CommandType::ACTION_MOVE_TO_READY;
    }
    
    void set_pose(const Pose& pose) {
        pose_ = pose;
    }
    
    const Pose& get_pose() const {
        return pose_;
    }
};

#endif // COMMAND_TYPES_H