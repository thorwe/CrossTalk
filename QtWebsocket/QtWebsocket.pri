INCLUDEPATH += $$PWD \
    $$PWD/..

SOURCES += \
    $$PWD/QWsServer.cpp \
    $$PWD/QWsSocket.cpp \
    $$PWD/QWsHandshake.cpp \
    $$PWD/QWsFrame.cpp \
#    $$PWD/QTlsServer.cpp \
    $$PWD/functions.cpp \
    $$PWD/ServerThreaded/ServerThreaded.cpp \
    $$PWD/ServerThreaded/SocketThread.cpp

HEADERS += \
    $$PWD/QWsServer.h \
    $$PWD/QWsSocket.h \
    $$PWD/QWsHandshake.h \
    $$PWD/QWsFrame.h \
#    $$PWD/QTlsServer.h \
    $$PWD/functions.h \
    $$PWD/WsEnums.h \
    $$PWD/ServerThreaded/ServerThreaded.h \
    $$PWD/ServerThreaded/SocketThread.h
