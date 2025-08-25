#include <gtest/gtest.h>
#include <QCoreApplication>

int main(int argc, char *argv[])
{
    // 创建Qt应用程序实例
    QCoreApplication app(argc, argv);
    
    // 初始化Google Test
    ::testing::InitGoogleTest(&argc, argv);
    
    // 运行所有测试
    int result = RUN_ALL_TESTS();
    
    return result;
}