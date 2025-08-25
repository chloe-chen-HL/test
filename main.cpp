#include <QApplication>
#include <gtest/gtest.h>
#include <gmock/gmock.h>

int main(int argc, char *argv[])
{
    // 初始化Qt应用程序
    QApplication app(argc, argv);
    
    // 初始化Google Test和Google Mock
    ::testing::InitGoogleTest(&argc, argv);
    ::testing::InitGoogleMock(&argc, argv);
    
    // 运行所有测试
    int result = RUN_ALL_TESTS();
    
    return result;
}