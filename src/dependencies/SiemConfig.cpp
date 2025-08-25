#include "SiemConfig.h"
#include <QMap>
#include <QDebug>

// 简化的配置存储
static QMap<QString, QString> configMap;

QString gConfig(const QString& key)
{
    // 为测试提供默认值
    if (key.startsWith("readyj")) {
        int index = key.mid(6).toInt();
        return QString::number(index * 10.0); // 返回默认角度值
    }
    return configMap.value(key, "0");
}

double gConfig(const QString& key, double defaultValue)
{
    bool ok;
    double value = gConfig(key).toDouble(&ok);
    return ok ? value : defaultValue;
}