#!/bin/bash

# ToReadyState 测试项目构建脚本

echo "=== ToReadyState 单元测试构建脚本 ==="

# 检查必要工具
echo "检查必要工具..."

if ! command -v qmake &> /dev/null; then
    echo "错误: qmake 未找到。请安装Qt开发环境。"
    exit 1
fi

if ! command -v make &> /dev/null; then
    echo "错误: make 未找到。请安装构建工具。"
    exit 1
fi

# 检查gtest库
if ! ldconfig -p | grep -q libgtest; then
    echo "警告: libgtest 可能未安装。"
    echo "Ubuntu/Debian: sudo apt-get install libgtest-dev libgmock-dev"
    echo "CentOS/RHEL: sudo yum install gtest-devel gmock-devel"
    echo "macOS: brew install googletest"
fi

# 清理之前的构建
echo "清理之前的构建..."
rm -rf bin build Makefile *.o moc_*

# 创建必要目录
mkdir -p bin build/obj build/moc build/rcc build/ui

# 运行qmake
echo "运行qmake..."
qmake ToReadyStateTest.pro

if [ $? -ne 0 ]; then
    echo "错误: qmake 失败"
    exit 1
fi

# 编译项目
echo "编译项目..."
make -j$(nproc)

if [ $? -ne 0 ]; then
    echo "错误: 编译失败"
    exit 1
fi

echo "构建成功!"

# 运行测试
if [ -f "bin/ToReadyStateTest" ]; then
    echo ""
    echo "=== 运行测试 ==="
    ./bin/ToReadyStateTest
    
    if [ $? -eq 0 ]; then
        echo ""
        echo "✅ 所有测试通过!"
    else
        echo ""
        echo "❌ 部分测试失败"
        exit 1
    fi
else
    echo "错误: 可执行文件未找到"
    exit 1
fi

echo ""
echo "=== 构建完成 ==="