#pragma once
#include <memory>

class QEvent {
public:
    using Type = int;

    explicit QEvent(Type t) : type_(t) {}
    virtual ~QEvent() = default;

    Type type() const { return type_; }

    void accept() { accepted_ = true; }
    bool isAccepted() const { return accepted_; }

private:
    Type type_;
    bool accepted_ { false };
};

class Command;

class CommandEvent : public QEvent {
public:
    static constexpr Type EventType = 1001;

    explicit CommandEvent(std::shared_ptr<Command> cmd)
        : QEvent(EventType), cmd_(std::move(cmd)) {}

    std::shared_ptr<Command> get_command() const { return cmd_; }

private:
    std::shared_ptr<Command> cmd_;
};