#!/bin/bash

# ToReadyState 单元测试项目安装和运行脚本

echo "=== ToReadyState 单元测试项目安装和运行脚本 ==="

# 检查操作系统
if [[ "$OSTYPE" == "linux-gnu"* ]]; then
    echo "检测到 Linux 系统"
elif [[ "$OSTYPE" == "darwin"* ]]; then
    echo "检测到 macOS 系统"
else
    echo "不支持的操作系统: $OSTYPE"
    exit 1
fi

# 检查是否已安装必要的工具
echo "检查必要的工具..."

# 检查 qmake
if ! command -v qmake &> /dev/null; then
    echo "错误: 未找到 qmake，请安装 Qt 开发环境"
    exit 1
fi

# 检查 make
if ! command -v make &> /dev/null; then
    echo "错误: 未找到 make"
    exit 1
fi

# 检查 gcc/g++
if ! command -v g++ &> /dev/null; then
    echo "错误: 未找到 g++ 编译器"
    exit 1
fi

echo "基本工具检查完成"

# 安装 Google Test
echo "安装 Google Test..."

if [[ "$OSTYPE" == "linux-gnu"* ]]; then
    # Ubuntu/Debian
    if command -v apt-get &> /dev/null; then
        echo "使用 apt-get 安装 Google Test..."
        sudo apt-get update
        sudo apt-get install -y libgtest-dev cmake
        
        # 编译 Google Test
        echo "编译 Google Test..."
        cd /usr/src/gtest
        sudo cmake CMakeLists.txt
        sudo make
        sudo cp lib/*.a /usr/lib
        cd -
        
    # CentOS/RHEL
    elif command -v yum &> /dev/null; then
        echo "使用 yum 安装 Google Test..."
        sudo yum install -y gtest-devel cmake
        
    # Arch Linux
    elif command -v pacman &> /dev/null; then
        echo "使用 pacman 安装 Google Test..."
        sudo pacman -S --noconfirm gtest cmake
        
    else
        echo "警告: 无法自动安装 Google Test，请手动安装"
    fi
    
elif [[ "$OSTYPE" == "darwin"* ]]; then
    # macOS
    if command -v brew &> /dev/null; then
        echo "使用 Homebrew 安装 Google Test..."
        brew install googletest
    else
        echo "警告: 未找到 Homebrew，请手动安装 Google Test"
    fi
fi

# 检查 Google Test 是否安装成功
echo "验证 Google Test 安装..."
if pkg-config --exists gtest; then
    echo "Google Test 安装成功"
else
    echo "警告: Google Test 可能未正确安装，但继续尝试编译..."
fi

# 创建构建目录
echo "创建构建目录..."
mkdir -p build
cd build

# 生成 Makefile
echo "生成 Makefile..."
qmake ../ToReadyStateTest.pro

if [ $? -ne 0 ]; then
    echo "错误: qmake 失败"
    exit 1
fi

# 编译项目
echo "编译项目..."
make -j$(nproc)

if [ $? -ne 0 ]; then
    echo "错误: 编译失败"
    echo "请检查以下可能的问题："
    echo "1. Qt 开发环境是否正确安装"
    echo "2. Google Test 是否正确安装"
    echo "3. 编译器是否支持 C++11"
    exit 1
fi

echo "编译成功！"

# 运行测试
echo "运行单元测试..."
./ToReadyStateTest

if [ $? -eq 0 ]; then
    echo "所有测试通过！"
else
    echo "部分测试失败，请检查测试结果"
fi

# 运行特定测试（可选）
echo ""
echo "运行特定测试示例："
echo "./ToReadyStateTest --gtest_filter=ToReadyStateTest.Constructor"
echo "./ToReadyStateTest --gtest_filter=ToReadyStateTest.GetReadyPose"
echo "./ToReadyStateTest --gtest_filter=ToReadyStateTest.TooNear*"

# 生成测试报告（可选）
echo ""
echo "生成详细测试报告："
echo "./ToReadyStateTest --gtest_output=xml:test_results.xml"

echo ""
echo "=== 安装和运行完成 ==="
echo "项目已成功编译并运行测试"
echo "可以在 Qt Creator 中打开 ToReadyStateTest.pro 文件进行进一步开发"