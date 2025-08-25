#pragma once
#include <QEvent>
#include <memory>

// 自定义事件类型
class CommandEvent : public QEvent
{
public:
    static const QEvent::Type EventType = static_cast<QEvent::Type>(QEvent::User + 1);
    
    CommandEvent(CommandPtr command) 
        : QEvent(EventType), command_(command) {}
    
    CommandPtr get_command() const { return command_; }
    
private:
    CommandPtr command_;
};

class PedalEvent : public QEvent
{
public:
    static const QEvent::Type EventType = static_cast<QEvent::Type>(QEvent::User + 2);
    
    PedalEvent(bool pressed) 
        : QEvent(EventType), pressed_(pressed) {}
    
    bool get_pressed() const { return pressed_; }
    
private:
    bool pressed_;
};