QT += core testlib
QT -= gui

CONFIG += qt console warn_on depend_includepath testcase
CONFIG -= app_bundle

TEMPLATE = app

# 添加Google Test支持
LIBS += -lgtest -lgtest_main -lpthread

# 包含路径
INCLUDEPATH += src/dependencies

# 源文件
SOURCES += \
    src/dependencies/statebase.cpp \
    src/dependencies/toreadyposecommand.cpp \
    src/dependencies/readyposecommand.cpp \
    src/dependencies/pedalcontroller.cpp \
    src/dependencies/factory.cpp \
    src/dependencies/SiemConfig.cpp \
    src/toreadystate.cpp \
    test/toreadystate_test.cpp \
    test/test_main.cpp

# 头文件
HEADERS += \
    src/dependencies/statebase.h \
    src/dependencies/toreadystate.h \
    src/dependencies/toreadyposecommand.h \
    src/dependencies/readyposecommand.h \
    src/dependencies/EyeModule.h \
    src/dependencies/sgnioRTDEControl.h \
    src/dependencies/pedalcontroller.h \
    src/dependencies/factory.h \
    src/dependencies/SiemConfig.h

# 编译选项
DEFINES += QT_DEPRECATED_WARNINGS

# 测试配置
CONFIG(debug, debug|release) {
    DESTDIR = debug
} else {
    DESTDIR = release
}