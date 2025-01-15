QT -= gui
QT += serialport websockets
QT += sql
CONFIG += c++11 console
CONFIG -= app_bundle
TARGET = iClockPLC
# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        ChatServer.cpp \
        Database.cpp \
        GPIOClass.cpp \
        NetworkMng.cpp \
        SPI.cpp \
        function.cpp \
        iClockDisplay.cpp \
        iClockDualGPS.cpp \
        infoDisplay.cpp \
        linux_spi.cpp \
        main.cpp \
        sslechoserver.cpp

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    ChatServer.h \
    Database.h \
    GPIOClass.h \
    NetworkMng.h \
    SPI.h \
    iClockDisplay.h \
    iClockDualGPS.h \
    infoDisplay.h \
    linux_spi.h \
    sslechoserver.h

 LIBS += -lboost_system -lboost_chrono -lboost_thread -ludev
