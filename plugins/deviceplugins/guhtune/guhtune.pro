include(../../plugins.pri)

TARGET = $$qtLibraryTarget(guh_devicepluginguhtune)

QT += widgets

SOURCES +=                  \
    devicepluginguhtune.cpp \
    guhbutton.cpp           \
    guhencoder.cpp          \
    guhtuneui.cpp           \
    guhtouch.cpp \
    guhtune-ui-elements/clock.cpp \
    guhtune-ui-elements/itemwidget.cpp

HEADERS +=                  \
    devicepluginguhtune.h   \
    guhbutton.h             \
    guhencoder.h            \
    guhtuneui.h             \
    guhtouch.h \
    guhtune-ui-elements/clock.h \
    guhtune-ui-elements/itemwidget.h

RESOURCES += images.qrc     \

