#pragma once
#include <string>

enum class CommandType {
    ACTION_LIGHT_CONTROL,
    OTHER
};

class Command {
public:
    virtual ~Command() = default;
    virtual CommandType get_type() const = 0;

    static std::string type2string(CommandType type) {
        switch (type) {
            case CommandType::ACTION_LIGHT_CONTROL: return "ACTION_LIGHT_CONTROL";
            case CommandType::OTHER: return "OTHER";
        }
        return "UNKNOWN";
    }
};