#pragma once
#include <vector>
#include <memory>
#include <string>

class Command {
public:
    enum class Type {
        ACTION_MOVE_TO_READY,
        STATUS_SET_READY_POSE
    };
    
    virtual ~Command() = default;
    virtual Type get_type() const = 0;
    virtual std::string encode() const = 0;
    
    static std::string type2string(Type type);
};

using CommandPtr = std::shared_ptr<Command>;

class ToReadyPoseCommand : public Command {
public:
    ToReadyPoseCommand();
    Type get_type() const override { return Type::ACTION_MOVE_TO_READY; }
    std::string encode() const override;
    
    void set_pose(const std::vector<double>& pose);
    std::vector<double> get_pose() const;
    void set_status(bool status);
    bool get_status() const;
    
private:
    std::vector<double> pose_;
    bool status_ = false;
};

using ToReadyPoseCommandPtr = std::shared_ptr<ToReadyPoseCommand>;