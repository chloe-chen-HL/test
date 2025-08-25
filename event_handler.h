#ifndef EVENT_HANDLER_H
#define EVENT_HANDLER_H

#include <memory>
#include <QDebug>
#include "event_system.h"
#include "command_types.h"

class EventHandler {
private:
    std::shared_ptr<ToReadyPoseCommand> toReady_;
    
    Pose getReadyPose() const {
        // 返回一个默认的ready pose
        return {1.0, 2.0, 3.0, 0.1, 0.2, 0.3};
    }
    
public:
    bool handleEvent(QEvent* e) {
        if (e->type() == CommandEvent::EventType) {
            CommandEvent *event = static_cast<CommandEvent *>(e);
            auto com = event->get_command();
            qDebug() << "toready received com" << Command::type2string(com->get_type());

            if (com->get_type() == CommandType::ACTION_MOVE_TO_READY) {
                toReady_ = std::dynamic_pointer_cast<ToReadyPoseCommand>(com);
                toReady_->set_pose(getReadyPose());
                qDebug() << "toReady pose is" << toReady_->get_pose();
                event->accept();
                return true;
            }
        }
        return false;
    }
    
    std::shared_ptr<ToReadyPoseCommand> getToReady() const {
        return toReady_;
    }
};

#endif // EVENT_HANDLER_H