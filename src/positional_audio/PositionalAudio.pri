INCLUDEPATH += $$PWD \
    $$PWD/..

#include($$PWD/sse_server/sse_server.pri) {
#    DEFINES += USE_SSE_SERVER
#    !build_pass:message( "Server-Sent Events Server included." )
#}

HEADERS += \
    $$PWD/tsvr_obj.h \
    $$PWD/tsvr_universe.h \
    $$PWD/mod_positionalaudio.h \
    $$PWD/settings_positionalaudio.h \
    $$PWD/groupbox_positional_audio.h \
    $$PWD/config_positionalaudio.h \
    $$PWD/groupbox_positionalaudio_servers.h \
    $$PWD/definitions_positionalaudio.h \
    $$PWD/groupbox_positionalaudio_status.h \
    $$PWD/guildwarstwo.h \
    src/positional_audio/tsvr_definitions.h \
    src/positional_audio/tsvr_obj_self.h \
    src/positional_audio/tsvr_obj_other.h

SOURCES += \
    $$PWD/tsvr_obj.cpp \
    $$PWD/tsvr_universe.cpp \
    $$PWD/mod_positionalaudio.cpp \
    $$PWD/settings_positionalaudio.cpp \
    $$PWD/groupbox_positional_audio.cpp \
    $$PWD/config_positionalaudio.cpp \
    $$PWD/groupbox_positionalaudio_servers.cpp \
    $$PWD/groupbox_positionalaudio_status.cpp \
    $$PWD/guildwarstwo.cpp \
    src/positional_audio/tsvr_obj_self.cpp \
    src/positional_audio/tsvr_obj_other.cpp

FORMS += \
    $$PWD/groupbox_positional_audio.ui \
    $$PWD/config_positionalaudio.ui \
    $$PWD/groupbox_positionalaudio_servers.ui \
    $$PWD/groupbox_positionalaudio_status.ui

beta {
    HEADERS += \
        $$PWD/gw.h
    SOURCES += \
        $$PWD/gw.cpp
}
