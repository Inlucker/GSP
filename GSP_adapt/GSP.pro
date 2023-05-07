QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    CmdListWindow.cpp \
    Command.cpp \
    DataBase.cpp \
    DataBaseWindow.cpp \
    GSP.cpp \
    GSPResWindow.cpp \
    GSPWorker.cpp \
    LogReader.cpp \
    ReadLogsWorker.cpp \
    Sequence.cpp \
    Session.cpp \
    main.cpp \
    MainWindow.cpp

HEADERS += \
    CmdListWindow.h \
    Command.h \
    DataBase.h \
    DataBaseWindow.h \
    GSP.h \
    GSPResWindow.h \
    GSPWorker.h \
    LogReader.h \
    MainWindow.h \
    ReadLogsWorker.h \
    Sequence.h \
    Session.h

FORMS += \
    CmdListWindow.ui \
    DataBaseWindow.ui \
    GSPResWindow.ui \
    MainWindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
