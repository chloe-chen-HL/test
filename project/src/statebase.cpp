#include "statebase.h"
#include "lightcontrolcommand.h"
#include "statemachine.h"
#include "enumregistry.h"
#include "qobject.h"

StateBase::StateBase(StateType type, std::weak_ptr<StateMachine> stateMachine)
    : stateMachine_(std::move(stateMachine)), type_(type) {}

StateType StateBase::getStateType() const { return type_; }

bool StateBase::event(QEvent* e) {
    if (e->type() == CommandEvent::EventType) {
        CommandEvent* event = static_cast<CommandEvent*>(e);
        auto com = event->get_command();
        qDebug() << "base: received com" << Command::type2string(com->get_type());

        if (com->get_type() == CommandType::ACTION_LIGHT_CONTROL) {
            auto light = std::dynamic_pointer_cast<LightControlCommand>(com);
            if (getStateMachine() && getStateMachine()->get_needle()) {
                auto name = light->get_param();
                auto color = EnumRegistry<NeedleController::LightColor>::instance().fromString(name.toStdString());
                qDebug() << "the color will be set: " << name.toStdString() << " " << (int)color;
                getStateMachine()->get_needle()->setLightColor(color);
            }
            e->accept();
            return true;
        }
    }
    return QObject::event(e);
}

void StateBase::setStateMachine(std::weak_ptr<StateMachine> stateMachine) { stateMachine_ = std::move(stateMachine); }

std::shared_ptr<StateMachine> StateBase::getStateMachine() const { return stateMachine_.lock(); }

void StateBase::clearContext() {}

void StateBase::changeStatus(MoveStatus st) {
    std::lock_guard<std::mutex> lk(mutex_);
    curStatus_ = st;
}