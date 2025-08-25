#pragma once
#include <QString>

// 全局配置函数
QString gConfig(const QString& key);
double gConfig(const QString& key, double defaultValue = 0.0);