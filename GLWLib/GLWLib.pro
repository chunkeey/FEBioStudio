TEMPLATE = lib
CONFIG += debug qt qwidgets opengl staticlib warn_off c++14
QMAKE_CXX = g++
QMAKE_CXXFLAGS += -std=c++14 -O0
DEFINES += LINUX HAS_OCC TETLIBRARY HAS_NETGEN HAS_QUAZIP HAS_SSH
DESTDIR = ../build/lib
TARGET = glwlib
INCLUDEPATH += ../
QT += widgets

HEADERS += *.h
SOURCES = $$files(*.cpp)
SOURCES -= $$files(moc_*.cpp)
