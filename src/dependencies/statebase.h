#pragma once
#include <QObject>
#include <QEvent>
#include <QDebug>

enum class StateType {
    TOREADY,
    READY,
    WORKING,
    STOP
};

enum class MoveStatus {
    STOPING,
    MOVING,
    FINISHED
};

class StateBase : public QObject
{
    Q_OBJECT
public:
    explicit StateBase(StateType type);
    virtual ~StateBase() = default;
    
    virtual bool enter() = 0;
    virtual bool exit() = 0;
    virtual bool event(QEvent *e);
    
    StateType getType() const { return type_; }
    MoveStatus getStatus() const { return curStatus_; }
    
protected:
    virtual void clearContext();
    void changeStatus(MoveStatus status);
    class StateMachine* getStateMachine();
    
    StateType type_;
    MoveStatus curStatus_ = MoveStatus::STOPING;
};