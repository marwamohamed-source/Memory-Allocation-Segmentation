QT       += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
CONFIG += c++17
SOURCES += \
    main.cpp \
    mainwindow.cpp \
    memory.cpp
HEADERS += \
    mainwindow.h \
    memory.h \
    process.h \
    segment.h
FORMS += \
    mainwindow.ui