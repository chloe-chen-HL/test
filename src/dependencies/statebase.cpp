#include "statebase.h"

StateBase::StateBase(StateType type) : type_(type)
{
}

bool StateBase::event(QEvent *e)
{
    return QObject::event(e);
}

void StateBase::clearContext()
{
    curStatus_ = MoveStatus::STOPING;
}

void StateBase::changeStatus(MoveStatus status)
{
    curStatus_ = status;
}

class StateMachine* StateBase::getStateMachine()
{
    // 简化实现，实际应该返回状态机实例
    return nullptr;
}