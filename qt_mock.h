#ifndef QT_MOCK_H
#define QT_MOCK_H

#include <iostream>
#include <sstream>

// 简单的QDebug模拟，用于测试
class QDebugMock {
private:
    std::ostringstream stream_;
    
public:
    template<typename T>
    QDebugMock& operator<<(const T& value) {
        stream_ << value << " ";
        return *this;
    }
    
    // 特殊处理Pose结构体的输出
    QDebugMock& operator<<(const struct Pose& pose) {
        stream_ << "Pose{" << pose.x << "," << pose.y << "," << pose.z 
                << "," << pose.roll << "," << pose.pitch << "," << pose.yaw << "}";
        return *this;
    }
    
    ~QDebugMock() {
        // 可以在这里输出到标准输出或者保存到某个地方用于测试验证
        // std::cout << stream_.str() << std::endl;
    }
    
    std::string str() const {
        return stream_.str();
    }
};

// 定义qDebug宏，在测试环境中使用mock版本
#define qDebug() QDebugMock()

#endif // QT_MOCK_H