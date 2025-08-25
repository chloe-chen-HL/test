# Makefile for GTest unit tests (without CMake)

# 编译器设置
CXX = g++
CXXFLAGS = -std=c++14 -Wall -Wextra -g -O0

# 包含路径
INCLUDES = -I. -I/usr/include/gtest -I/usr/include/gmock

# 库链接
LIBS = -lgtest -lgmock -lpthread

# 目标文件
TARGET = test_event_handler
SOURCES = test_event_handler.cpp

# 对象文件
OBJECTS = $(SOURCES:.cpp=.o)

# 默认目标
all: $(TARGET)

# 编译测试程序
$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LIBS)

# 编译对象文件
%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# 运行测试
test: $(TARGET)
	./$(TARGET)

# 运行测试并显示详细输出
test-verbose: $(TARGET)
	./$(TARGET) --gtest_output=xml:test_results.xml --gtest_print_time=1

# 检查GTest和GMock是否安装
check-deps:
	@echo "Checking for GTest and GMock..."
	@pkg-config --exists gtest && echo "GTest found" || echo "GTest not found - install with: sudo apt-get install libgtest-dev"
	@pkg-config --exists gmock && echo "GMock found" || echo "GMock not found - install with: sudo apt-get install libgmock-dev"

# 安装依赖（Ubuntu/Debian）
install-deps:
	sudo apt-get update
	sudo apt-get install -y libgtest-dev libgmock-dev
	# 如果需要编译GTest源码（某些系统上）
	# cd /usr/src/gtest && sudo cmake . && sudo make && sudo cp *.a /usr/lib

# 清理
clean:
	rm -f $(OBJECTS) $(TARGET) test_results.xml

# 重新编译
rebuild: clean all

# 代码格式化（如果有clang-format）
format:
	@if command -v clang-format >/dev/null 2>&1; then \
		clang-format -i *.cpp *.h; \
		echo "Code formatted"; \
	else \
		echo "clang-format not found, skipping formatting"; \
	fi

# 静态分析（如果有cppcheck）
analyze:
	@if command -v cppcheck >/dev/null 2>&1; then \
		cppcheck --enable=all --std=c++14 *.cpp *.h; \
	else \
		echo "cppcheck not found, skipping static analysis"; \
	fi

# 显示帮助
help:
	@echo "Available targets:"
	@echo "  all          - Build the test executable"
	@echo "  test         - Build and run tests"
	@echo "  test-verbose - Build and run tests with verbose output"
	@echo "  check-deps   - Check if GTest and GMock are installed"
	@echo "  install-deps - Install GTest and GMock dependencies"
	@echo "  clean        - Remove built files"
	@echo "  rebuild      - Clean and build"
	@echo "  format       - Format code with clang-format"
	@echo "  analyze      - Run static analysis with cppcheck"
	@echo "  help         - Show this help message"

# 声明伪目标
.PHONY: all test test-verbose check-deps install-deps clean rebuild format analyze help