
contains(DEFINES, WEBSOCKET){
    QT += websockets
    SOURCES += $$top_srcdir/server/websocketserver.cpp
    HEADERS += $$top_srcdir/server/websocketserver.h
}

RESOURCES += $$top_srcdir/icons.qrc

SOURCES += $$top_srcdir/server/guhcore.cpp \
    $$top_srcdir/server/tcpserver.cpp \
    $$top_srcdir/server/ruleengine.cpp \
    $$top_srcdir/server/rule.cpp \
    $$top_srcdir/server/jsonrpc/jsonrpcserver.cpp \
    $$top_srcdir/server/jsonrpc/jsonhandler.cpp \
    $$top_srcdir/server/jsonrpc/devicehandler.cpp \
    $$top_srcdir/server/jsonrpc/jsontypes.cpp \
    $$top_srcdir/server/jsonrpc/ruleshandler.cpp \
    $$top_srcdir/server/jsonrpc/actionhandler.cpp \
    $$top_srcdir/server/jsonrpc/eventhandler.cpp \
    $$top_srcdir/server/jsonrpc/statehandler.cpp \
    $$top_srcdir/server/jsonrpc/logginghandler.cpp \
    $$top_srcdir/server/jsonrpc/authenticationhandler.cpp \
    $$top_srcdir/server/stateevaluator.cpp \
    $$top_srcdir/server/logging/logengine.cpp \
    $$top_srcdir/server/logging/logfilter.cpp \
    $$top_srcdir/server/logging/logentry.cpp \
    $$top_srcdir/server/webserver.cpp \
    $$top_srcdir/server/transportinterface.cpp \
    $$top_srcdir/server/servermanager.cpp \
    $$top_srcdir/server/httprequest.cpp \
    $$top_srcdir/server/httpreply.cpp \
    $$top_srcdir/server/rest/restserver.cpp \
    $$top_srcdir/server/rest/restresource.cpp \
    $$top_srcdir/server/rest/devicesresource.cpp \
    $$top_srcdir/server/rest/deviceclassesresource.cpp \
    $$top_srcdir/server/rest/vendorsresource.cpp \
    $$top_srcdir/server/rest/logsresource.cpp \
    $$top_srcdir/server/rest/pluginsresource.cpp \
    $$top_srcdir/server/rest/rulesresource.cpp \
    $$top_srcdir/server/authenticationmanager.cpp \
    $$top_srcdir/server/authentication/authorizedconnection.cpp \
    $$top_srcdir/server/authentication/user.cpp


HEADERS += $$top_srcdir/server/guhcore.h \
    $$top_srcdir/server/tcpserver.h \
    $$top_srcdir/server/ruleengine.h \
    $$top_srcdir/server/rule.h \
    $$top_srcdir/server/jsonrpc/jsonrpcserver.h \
    $$top_srcdir/server/jsonrpc/jsonhandler.h \
    $$top_srcdir/server/jsonrpc/devicehandler.h \
    $$top_srcdir/server/jsonrpc/jsontypes.h \
    $$top_srcdir/server/jsonrpc/ruleshandler.h \
    $$top_srcdir/server/jsonrpc/actionhandler.h \
    $$top_srcdir/server/jsonrpc/eventhandler.h \
    $$top_srcdir/server/jsonrpc/statehandler.h \
    $$top_srcdir/server/jsonrpc/logginghandler.h \
    $$top_srcdir/server/jsonrpc/authenticationhandler.h \
    $$top_srcdir/server/stateevaluator.h \
    $$top_srcdir/server/logging/logging.h \
    $$top_srcdir/server/logging/logengine.h \
    $$top_srcdir/server/logging/logfilter.h \
    $$top_srcdir/server/logging/logentry.h \
    $$top_srcdir/server/webserver.h \
    $$top_srcdir/server/transportinterface.h \
    $$top_srcdir/server/servermanager.h \
    $$top_srcdir/server/httprequest.h \
    $$top_srcdir/server/httpreply.h \
    $$top_srcdir/server/rest/restserver.h \
    $$top_srcdir/server/rest/restresource.h \
    $$top_srcdir/server/rest/devicesresource.h \
    $$top_srcdir/server/rest/deviceclassesresource.h \
    $$top_srcdir/server/rest/vendorsresource.h \
    $$top_srcdir/server/rest/logsresource.h \
    $$top_srcdir/server/rest/pluginsresource.h \
    $$top_srcdir/server/rest/rulesresource.h \
    $$top_srcdir/server/authenticationmanager.h \
    $$top_srcdir/server/authentication/authorizedconnection.h \
    $$top_srcdir/server/authentication/user.h


