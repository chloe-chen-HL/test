#pragma once
#include <vector>
#include <string>
#include <memory>
#include <gmock/gmock.h>

// 命令基类接口
class Command
{
public:
    virtual ~Command() = default;
    virtual CommandType get_type() const = 0;
    virtual std::string encode() const = 0;
    
    static std::string type2string(CommandType type) {
        switch(type) {
            case CommandType::ACTION_MOVE_TO_READY: return "ACTION_MOVE_TO_READY";
            case CommandType::STATUS_SET_READY_POSE: return "STATUS_SET_READY_POSE";
            default: return "UNKNOWN";
        }
    }
};

// ToReadyPoseCommand接口
class ToReadyPoseCommand : public Command
{
public:
    virtual void set_pose(const std::vector<double>& pose) = 0;
    virtual std::vector<double> get_pose() const = 0;
    virtual void set_status(bool status) = 0;
    virtual bool get_status() const = 0;
};

// ReadyPoseCommand接口
class ReadyPoseCommand : public Command
{
public:
    virtual void set_pose(const std::vector<double>& pose) = 0;
    virtual std::vector<double> get_pose() const = 0;
};

// 模拟实现
class MockToReadyPoseCommand : public ToReadyPoseCommand
{
public:
    MOCK_METHOD(CommandType, get_type, (), (const, override));
    MOCK_METHOD(std::string, encode, (), (const, override));
    MOCK_METHOD(void, set_pose, (const std::vector<double>& pose), (override));
    MOCK_METHOD(std::vector<double>, get_pose, (), (const, override));
    MOCK_METHOD(void, set_status, (bool status), (override));
    MOCK_METHOD(bool, get_status, (), (const, override));
};

class MockReadyPoseCommand : public ReadyPoseCommand
{
public:
    MOCK_METHOD(CommandType, get_type, (), (const, override));
    MOCK_METHOD(std::string, encode, (), (const, override));
    MOCK_METHOD(void, set_pose, (const std::vector<double>& pose), (override));
    MOCK_METHOD(std::vector<double>, get_pose, (), (const, override));
};

// 智能指针类型定义
using CommandPtr = std::shared_ptr<Command>;
using ToReadyPoseCommandPtr = std::shared_ptr<ToReadyPoseCommand>;
using ReadyPoseCommandPtr = std::shared_ptr<ReadyPoseCommand>;

// 命令工厂接口
class CommandFactory
{
public:
    virtual ~CommandFactory() = default;
    virtual CommandPtr lastCommand(CommandType type) = 0;
};

class MockCommandFactory : public CommandFactory
{
public:
    MOCK_METHOD(CommandPtr, lastCommand, (CommandType type), (override));
};

// 单例模板
template<typename T>
class Singleton
{
public:
    static T& instance() {
        static T instance_;
        return instance_;
    }
};

// 输出队列接口
class OutputQueue
{
public:
    virtual ~OutputQueue() = default;
    virtual void put(const std::string& data) = 0;
};

class MockOutputQueue : public OutputQueue
{
public:
    MOCK_METHOD(void, put, (const std::string& data), (override));
};

// 状态机接口
class StateMachine
{
public:
    virtual ~StateMachine() = default;
    virtual OutputQueue* get_outputQue() = 0;
};

class MockStateMachine : public StateMachine
{
public:
    MOCK_METHOD(OutputQueue*, get_outputQue, (), (override));
};

// 配置函数模拟
class ConfigManager
{
public:
    static QString gConfig(const QString& key) {
        // 返回模拟的配置值
        if (key.startsWith("readyj")) {
            return "0.0"; // 默认关节角度
        }
        return "";
    }
};

// 全局配置函数
inline QString gConfig(const QString& key) {
    return ConfigManager::gConfig(key);
}