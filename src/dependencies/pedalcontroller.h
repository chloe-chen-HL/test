#pragma once
#include <QEvent>

class PedalEvent : public QEvent {
public:
    static const QEvent::Type EventType = static_cast<QEvent::Type>(QEvent::User + 1);
    
    PedalEvent(bool pressed);
    bool get_pressed() const;
    
private:
    bool pressed_;
};

class CommandEvent : public QEvent {
public:
    static const QEvent::Type EventType = static_cast<QEvent::Type>(QEvent::User + 2);
    
    CommandEvent(class CommandPtr command);
    class CommandPtr get_command() const;
    
private:
    class CommandPtr command_;
};