QT += core testlib
QT -= gui

CONFIG += c++17 console
CONFIG -= app_bundle

# 项目名称
TARGET = ToReadyStateTest
TEMPLATE = app

# 源文件
SOURCES += \
    main.cpp \
    test_toreadystate.cpp \
    toreadystate.cpp \
    mocks/mock_statebase.cpp \
    mocks/mock_rtde.cpp \
    mocks/mock_eyemodule.cpp \
    mocks/mock_command.cpp

# 头文件
HEADERS += \
    toreadystate.h \
    mocks/mock_statebase.h \
    mocks/mock_rtde.h \
    mocks/mock_eyemodule.h \
    mocks/mock_command.h \
    mocks/mock_events.h

# 包含路径
INCLUDEPATH += \
    . \
    mocks \
    /usr/include/gtest \
    /usr/include/gmock

# 链接库
LIBS += -lgtest -lgmock -pthread

# 定义
DEFINES += QT_DEPRECATED_WARNINGS

# 输出目录
DESTDIR = $$PWD/bin
OBJECTS_DIR = $$PWD/build/obj
MOC_DIR = $$PWD/build/moc
RCC_DIR = $$PWD/build/rcc
UI_DIR = $$PWD/build/ui