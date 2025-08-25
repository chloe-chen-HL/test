# Makefile for gtest unit tests
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -g
GTEST_DIR = /usr/include/gtest
GTEST_LIB = -lgtest -lgtest_main -lgmock -lpthread

# Qt相关
QT_INCLUDE = $(shell pkg-config --cflags Qt5Core)
QT_LIB = $(shell pkg-config --libs Qt5Core)

# 如果没有pkg-config，可以手动指定Qt路径
# QT_INCLUDE = -I/usr/include/qt5/QtCore -I/usr/include/qt5
# QT_LIB = -lQt5Core

# 目标文件
TARGET = test_command_event_handler
TEST_SOURCE = test_command_event_handler.cpp

# 默认目标
all: $(TARGET)

# 编译测试程序
$(TARGET): $(TEST_SOURCE)
	$(CXX) $(CXXFLAGS) $(QT_INCLUDE) -I$(GTEST_DIR) -o $@ $< $(QT_LIB) $(GTEST_LIB)

# 运行测试
test: $(TARGET)
	./$(TARGET)

# 运行测试并显示详细信息
test-verbose: $(TARGET)
	./$(TARGET) --gtest_verbose

# 运行特定测试
test-specific: $(TARGET)
	./$(TARGET) --gtest_filter=CommandEventHandlerTest.HandleActionMoveToReadyCommand

# 清理
clean:
	rm -f $(TARGET)

# 安装依赖（Ubuntu/Debian）
install-deps:
	sudo apt-get update
	sudo apt-get install -y libgtest-dev libgmock-dev libqt5core5a qt5-qmake

# 安装依赖（CentOS/RHEL/Fedora）
install-deps-rpm:
	sudo yum install -y gtest-devel gmock-devel qt5-qtbase-devel

# 帮助信息
help:
	@echo "可用的目标:"
	@echo "  all          - 编译测试程序"
	@echo "  test         - 运行所有测试"
	@echo "  test-verbose - 运行测试并显示详细信息"
	@echo "  test-specific- 运行特定测试"
	@echo "  clean        - 清理编译文件"
	@echo "  install-deps - 安装依赖（Ubuntu/Debian）"
	@echo "  install-deps-rpm - 安装依赖（CentOS/RHEL/Fedora）"
	@echo "  help         - 显示此帮助信息"

.PHONY: all test test-verbose test-specific clean install-deps install-deps-rpm help