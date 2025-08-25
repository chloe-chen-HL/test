#ifndef EVENT_SYSTEM_H
#define EVENT_SYSTEM_H

#include <memory>
#include "command_types.h"

class QEvent {
public:
    virtual ~QEvent() = default;
    virtual int type() const = 0;
    virtual void accept() = 0;
    virtual void ignore() = 0;
};

class CommandEvent : public QEvent {
public:
    static const int EventType = 1001;
    
private:
    std::shared_ptr<Command> command_;
    bool accepted_ = false;
    
public:
    CommandEvent(std::shared_ptr<Command> command) : command_(command) {}
    
    int type() const override {
        return EventType;
    }
    
    void accept() override {
        accepted_ = true;
    }
    
    void ignore() override {
        accepted_ = false;
    }
    
    bool isAccepted() const {
        return accepted_;
    }
    
    std::shared_ptr<Command> get_command() const {
        return command_;
    }
};

#endif // EVENT_SYSTEM_H