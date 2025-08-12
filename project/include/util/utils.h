#pragma once
#include <string>

class QString {
public:
    QString() = default;
    QString(const char* s) : data_(s) {}
    QString(const std::string& s) : data_(s) {}

    std::string toStdString() const { return data_; }

private:
    std::string data_;
};