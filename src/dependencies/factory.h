#pragma once
#include "toreadyposecommand.h"
#include <memory>

template<typename T>
class Singleton {
public:
    static T& instance() {
        static T instance;
        return instance;
    }
    
protected:
    Singleton() = default;
    ~Singleton() = default;
    Singleton(const Singleton&) = delete;
    Singleton& operator=(const Singleton&) = delete;
};

class CommandFactory : public Singleton<CommandFactory> {
public:
    CommandPtr lastCommand(Command::Type type);
    void setLastCommand(CommandPtr command);
    
private:
    CommandPtr lastCommand_;
};