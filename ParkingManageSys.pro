QT       += core gui multimedia multimediawidgets sql network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0
INCLUDEPATH += hyperlpr3/include
#HyperLPR
LIBS += $$PWD/hyperlpr3/lib/hyperlpr3.lib
LIBS += $$PWD/hyperlpr3/lib/MNN.lib
LIBS += $$PWD/hyperlpr3/lib/opencv_world4120.lib
LIBS += $$PWD/hyperlpr3/lib/opencv_world4120d.lib
LIBS += $$PWD/hyperlpr3/lib/pthread.lib
#OpenSSL
LIBS += $$PWD/openssl/libcrypto.a
LIBS += $$PWD/openssl/libssl.a
LIBS += -lcrypt32
LIBS += -ladvapi32
LIBS += -lgcc -lws2_32 -lgdi32
LIBS += -static-libgcc
LIBS += -static-libstdc++
#OpenCV
LIBS += $$PWD/opencv2/lib/libopencv_core4120.dll.a
LIBS += $$PWD/opencv2/lib/libopencv_imgproc4120.dll.a
LIBS += $$PWD/opencv2/lib/libopencv_imgcodecs4120.dll
LIBS += $$PWD/opencv2/lib/libopencv_objdetect4120.dll

SOURCES += \
    bussinessdataviewer.cpp \
    databasemanager.cpp \
    main.cpp \
    mainwindow.cpp \
    paymentservice.cpp

HEADERS += \
    bussinessdataviewer.h \
    databasemanager.h \
    mainwindow.h \
    paymentservice.h

FORMS += \
    bussinessdataviewer.ui \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
