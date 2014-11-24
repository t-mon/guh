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
    guhtune-ui-elements/itemwidget.cpp \
    guhtune-ui-elements/itemone.cpp \
    guhtune-ui-elements/itemtwo.cpp \
    guhtune-ui-elements/itemthree.cpp \
    guhtune-ui-elements/itemfour.cpp

HEADERS +=                  \
    devicepluginguhtune.h   \
    guhbutton.h             \
    guhencoder.h            \
    guhtuneui.h             \
    guhtouch.h \
    guhtune-ui-elements/clock.h \
    guhtune-ui-elements/itemwidget.h \
    guhtune-ui-elements/itemone.h \
    guhtune-ui-elements/itemtwo.h \
    guhtune-ui-elements/itemthree.h \
    guhtune-ui-elements/itemfour.h

RESOURCES += images.qrc     \
