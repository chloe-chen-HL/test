#pragma once
#include <memory>
#include <string>

// Mock QEvent and related classes
class QEvent
{
public:
    enum Type {
        None = 0,
        User = 1000
    };
    
    QEvent(Type type) : type_(type) {}
    virtual ~QEvent() = default;
    Type type() const { return type_; }
    void accept() { accepted_ = true; }
    bool isAccepted() const { return accepted_; }
    
private:
    Type type_;
    bool accepted_ = false;
};

// Mock QObject
class QObject
{
public:
    QObject() = default;
    virtual ~QObject() = default;
    virtual bool event(QEvent* e) { return false; }
};

// Mock Command types
enum class CommandType
{
    ACTION_LIGHT_CONTROL,
    OTHER_COMMAND
};

class Command
{
public:
    virtual ~Command() = default;
    virtual CommandType get_type() const = 0;
    static QString type2string(CommandType type)
    {
        switch(type)
        {
        case CommandType::ACTION_LIGHT_CONTROL:
            return "ACTION_LIGHT_CONTROL";
        default:
            return "UNKNOWN";
        }
    }
};

class LightControlCommand : public Command
{
public:
    LightControlCommand(const QString& param) : param_(param) {}
    CommandType get_type() const override { return CommandType::ACTION_LIGHT_CONTROL; }
    QString get_param() const { return param_; }
    
private:
    QString param_;
};

// Mock CommandEvent
class CommandEvent : public QEvent
{
public:
    static const Type EventType = static_cast<Type>(User + 1);
    
    CommandEvent(std::shared_ptr<Command> cmd) 
        : QEvent(EventType), command_(cmd) {}
    
    std::shared_ptr<Command> get_command() const { return command_; }
    
private:
    std::shared_ptr<Command> command_;
};

// Mock NeedleController
namespace NeedleController
{
    enum class LightColor
    {
        RED,
        GREEN,
        BLUE,
        WHITE,
        OFF
    };
}

class MockNeedle
{
public:
    void setLightColor(NeedleController::LightColor color) 
    { 
        lastColor_ = color; 
        colorSetCount_++;
    }
    
    NeedleController::LightColor getLastColor() const { return lastColor_; }
    int getColorSetCount() const { return colorSetCount_; }
    void reset() { colorSetCount_ = 0; lastColor_ = NeedleController::LightColor::OFF; }
    
private:
    NeedleController::LightColor lastColor_ = NeedleController::LightColor::OFF;
    int colorSetCount_ = 0;
};

// Mock StateMachine
class StateMachine
{
public:
    StateMachine() : needle_(std::make_shared<MockNeedle>()) {}
    
    std::shared_ptr<MockNeedle> get_needle() { return needle_; }
    
private:
    std::shared_ptr<MockNeedle> needle_;
};

// Mock EnumRegistry
template<typename T>
class EnumRegistry
{
public:
    static EnumRegistry& instance()
    {
        static EnumRegistry inst;
        return inst;
    }
    
    T fromString(const std::string& str)
    {
        if (str == "RED") return static_cast<T>(0);
        if (str == "GREEN") return static_cast<T>(1);
        if (str == "BLUE") return static_cast<T>(2);
        if (str == "WHITE") return static_cast<T>(3);
        return static_cast<T>(4); // OFF
    }
};

// Mock QString
class QString
{
public:
    QString() = default;
    QString(const char* str) : data_(str) {}
    QString(const std::string& str) : data_(str) {}
    
    std::string toStdString() const { return data_; }
    
    bool operator==(const QString& other) const { return data_ == other.data_; }
    bool operator!=(const QString& other) const { return data_ != other.data_; }
    
private:
    std::string data_;
};

// Mock qDebug
class QDebug
{
public:
    QDebug& operator<<(const QString& str) { return *this; }
    QDebug& operator<<(const char* str) { return *this; }
    QDebug& operator<<(int val) { return *this; }
    QDebug& operator<<(const std::string& str) { return *this; }
};

inline QDebug qDebug() { return QDebug(); }