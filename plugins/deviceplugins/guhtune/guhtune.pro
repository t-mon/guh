include(../../plugins.pri)

TARGET = $$qtLibraryTarget(guh_devicepluginguhtune)

QT += widgets

SOURCES +=                  \
    devicepluginguhtune.cpp \
    guhbutton.cpp           \
    guhencoder.cpp          \
    guhtuneui.cpp           \
    guhtouch.cpp            \

HEADERS +=                  \
    devicepluginguhtune.h   \
    guhbutton.h             \
    guhencoder.h            \
    guhtuneui.h             \
    guhtouch.h              \

RESOURCES += images.qrc     \

