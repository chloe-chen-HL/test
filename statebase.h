#pragma once
#ifdef UNIT_TEST_BUILD
#include "mock_classes.h"
#else
#include "event.h"
#include "qobject.h"
#endif

#include <memory>
#include <mutex>
#include "util/utils.h"

class StateMachine;

// State enum
enum class StateType
{
    IDLE,
    READY,
    T1GEN,
    TOREADY,
    TOSURFACE,
    ADJUST,
    URDRAG,
    NEEDLE,
};

enum class MoveStatus
{
    READY,
    STOPING,
    MOVING,
    FINISHED_ONE_STEP,
    FINISHED
};

/**
 * @brief Base class for all states
 */
class StateBase : public QObject
{
public:
    StateBase(StateType type, std::weak_ptr<StateMachine> stateMachine = {});
    virtual ~StateBase() = default;
    virtual bool enter() = 0;
    virtual bool exit() = 0;
    StateType getStateType() const;
    void setStateMachine(std::weak_ptr<StateMachine> stateMachine);
    std::shared_ptr<StateMachine> getStateMachine() const;

protected:
    virtual bool event(QEvent *e) override;
    virtual void clearContext();
    void changeStatus(MoveStatus);

protected:
    MoveStatus curStatus_ = MoveStatus::STOPING;
    StateType type_;

private:
    std::weak_ptr<StateMachine> stateMachine_;
    mutable std::mutex mutex_;
};

using StateBasePtr = std::shared_ptr<StateBase>;
using StateBaseWeakPtr = std::weak_ptr<StateBase>;