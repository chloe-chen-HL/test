#pragma once
#include <QObject>
#include <QEvent>
#include <QDebug>
#include <gmock/gmock.h>

// 状态类型枚举
enum class StateType {
    TOREADY,
    READY,
    STOP
};

// 移动状态枚举
enum class MoveStatus {
    STOPING,
    MOVING,
    FINISHED
};

// 命令类型枚举
enum class CommandType {
    ACTION_MOVE_TO_READY,
    STATUS_SET_READY_POSE
};

// 前向声明
class StateMachine;

// StateBase基类的模拟实现
class StateBase : public QObject
{
    Q_OBJECT

public:
    StateBase(StateType type) : type_(type), curStatus_(MoveStatus::STOPING) {}
    virtual ~StateBase() = default;

    virtual bool enter() = 0;
    virtual bool exit() = 0;
    virtual bool event(QEvent *e) { return QObject::event(e); }
    virtual void clearContext() = 0;

    StateType getType() const { return type_; }
    MoveStatus getCurrentStatus() const { return curStatus_; }
    
    void changeStatus(MoveStatus status) { 
        curStatus_ = status; 
        qDebug() << "Status changed to:" << static_cast<int>(status);
    }

    StateMachine* getStateMachine() { return stateMachine_; }
    void setStateMachine(StateMachine* sm) { stateMachine_ = sm; }

protected:
    StateType type_;
    MoveStatus curStatus_;
    StateMachine* stateMachine_ = nullptr;
};