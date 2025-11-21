QT       += core gui multimedia multimediawidgets sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0
INCLUDEPATH += hyperlpr3/include
MNN_LIB_DIR = $$PWD/hyperlpr3/lib
LIBS += -L$$MNN_LIB_DIR
LIBS += C:\MNN.lib
LIBS += C:\hyperlpr3.lib
LIBS += C:\opencv_world4120.lib
LIBS += C:\opencv_world4120d.lib
LIBS += C:\pthread.lib

SOURCES += \
    bussinessdataviewer.cpp \
    databasemanager.cpp \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    bussinessdataviewer.h \
    databasemanager.h \
    mainwindow.h

FORMS += \
    bussinessdataviewer.ui \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
