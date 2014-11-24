include(../../plugins.pri)

TARGET = $$qtLibraryTarget(guh_devicepluginguhtune)

QT += network

SOURCES +=                  \
    devicepluginguhtune.cpp \
    guhbutton.cpp           \
    guhencoder.cpp          \
    guhtuneuiserver.cpp     \
    guhtouch.cpp            \

HEADERS +=                  \
    devicepluginguhtune.h   \
    guhbutton.h             \
    guhencoder.h            \
    guhtuneuiserver.h       \
    guhtouch.h              \
