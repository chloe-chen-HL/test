#include "pedalcontroller.h"

PedalEvent::PedalEvent(bool pressed) : QEvent(EventType), pressed_(pressed)
{
}

bool PedalEvent::get_pressed() const
{
    return pressed_;
}

CommandEvent::CommandEvent(CommandPtr command) : QEvent(EventType), command_(command)
{
}

CommandPtr CommandEvent::get_command() const
{
    return command_;
}