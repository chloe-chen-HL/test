#include "toreadystate.h"
#include <QDebug>
#include <QApplication>
#include <iostream>

ToReadyState::ToReadyState(RtdePtr rtde, EyeModulePtr eye) 
    : StateBase(StateType::TOREADY), rtde_(rtde), eye_(eye)
{
}

bool ToReadyState::enter()
{
    if (rtde_ == nullptr || eye_ == nullptr)
    {
        qDebug() << "ToReadyState state init failed";
        return false;
    }
    // if previous state is not STOP
    clearContext();
    qDebug() << "enter toready state";
    return true;
}

bool ToReadyState::exit()
{
    qDebug() << "exit toready state";
    return true;
}

bool ToReadyState::event(QEvent *e)
{
    if (e->type() == CommandEvent::EventType)
    {
        CommandEvent *event = static_cast<CommandEvent *>(e);
        auto com = event->get_command();
        qDebug() <<"toready received com" << Command::type2string(com->get_type()).c_str();

        if (com->get_type() == CommandType::ACTION_MOVE_TO_READY)
        {
            toReady_ = std::dynamic_pointer_cast<ToReadyPoseCommand>(com);
            toReady_->set_pose(getReadyPose());
            auto pose = toReady_->get_pose();
            qDebug() << "toReady pose size:" << pose.size();
            event->accept();
            return true;
        }
    }
    else if (e->type() == PedalEvent::EventType)
    {
        PedalEvent *event = static_cast<PedalEvent *>(e);
        if (toReady_ && event && event->get_pressed())
        {
            if (curStatus_ == MoveStatus::STOPING)
            {
                // qDebug() << "pedal in ready state pressed";
                auto p = toReady_->get_pose();
                if (!p.empty())
                {
                    qDebug() << "to ready,pose size:" << p.size();
                    rtde_->movel(p);
                    changeStatus(MoveStatus::MOVING);
                }
            }

            urProgress(rtde_->getAsyncProgress());
        }
        else if (toReady_ && event && !event->get_pressed())
        {
            if (curStatus_ == MoveStatus::MOVING)
            {
                qDebug() << "pedal in ready state released";
                rtde_->stopl();
                changeStatus(MoveStatus::STOPING);
            }
        }
        event->accept();
        return true;
    }

    return StateBase::event(e);
}

void ToReadyState::urProgress(int p)
{
    //qDebug() << "p is " << p;
    if (p > 0)
    {
    }
    if (p < 0 && curStatus_ == MoveStatus::MOVING)
    {
        qDebug() << "ready finished";
        changeStatus(MoveStatus::FINISHED);
        toReady_->set_status(true);
        qDebug() << __FUNCTION__ << "toready feedback";
        try
        {
            if (getStateMachine() && getStateMachine()->get_outputQue()) {
                getStateMachine()->get_outputQue()->put(toReady_->encode()); // feedback command result
            }
        }
        catch (const std::exception &e)
        {
            std::cerr << e.what() << '\n';
        }
    }
}

void ToReadyState::clearContext()
{
    toReady_ = nullptr;
    curStatus_ = MoveStatus::STOPING;
}

std::vector<double> ToReadyState::getReadyPose()
{
    std::vector<double> res(6, 0);
    for (int i = 0; i < 6; i++)
    {
        const double factor = M_PI / 180;
        QString pos = QString("readyj%1").arg(i + 1);
        res[i] = gConfig(pos).toDouble();
    }

    CommandPtr p = Singleton<MockCommandFactory>::instance().lastCommand(CommandType::STATUS_SET_READY_POSE);
    ReadyPoseCommandPtr poscom = std::dynamic_pointer_cast<ReadyPoseCommand>(p);
    if (poscom)
    {
        res = poscom->get_pose();
    }
    return res;
}

bool ToReadyState::tooNear(std::vector<double> &p1, std::vector<double> &p2)
{
    double pos_diff = 10;
    if (p1.size() >= 3 && p2.size() >= 3)
    {
        pos_diff = sqrt(
            pow(p1[0] - p2[0], 2) +
            pow(p1[1] - p2[1], 2) +
            pow(p1[2] - p2[2], 2));
    }
    // qDebug()<<p1<<" "<<p2<<" "<<pos_diff;
    return pos_diff <= 0.01;
}

std::vector<std::string> ToReadyState::urScript()
{
    // 实现UR脚本生成逻辑
    std::vector<std::string> script;
    script.push_back("# UR Script for ToReady State");
    return script;
}

#include "toreadystate.moc"