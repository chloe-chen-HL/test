#pragma once
#include "command.h"
#include "util/utils.h"

class LightControlCommand : public Command {
public:
    explicit LightControlCommand(const QString& name) : name_(name) {}

    CommandType get_type() const override { return CommandType::ACTION_LIGHT_CONTROL; }
    QString get_param() const { return name_; }

private:
    QString name_;
};