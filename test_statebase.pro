QT += core testlib
QT -= gui

CONFIG += c++17 console
CONFIG -= app_bundle

TARGET = test_statebase
TEMPLATE = app

# GTest configuration
INCLUDEPATH += /usr/include/gtest
LIBS += -lgtest -lgtest_main -pthread

# Source files
SOURCES += \
    test_statebase.cpp \
    statebase.cpp \
    mock_classes.cpp

# Header files  
HEADERS += \
    statebase.h \
    mock_classes.h \
    event.h \
    qobject.h \
    statemachine.h \
    lightcontrolcommand.h \
    enumregistry.h

# Include paths
INCLUDEPATH += . \
               ../util

# Define for testing
DEFINES += UNIT_TEST_BUILD