INCLUDEPATH += $$PWD \
    $$PWD/..

HEADERS += \
    $$PWD/universe.h \
    $$PWD/mod_positionalaudio.h \
    $$PWD/settings_positionalaudio.h \
    $$PWD/groupbox_positional_audio.h \
    $$PWD/config_positionalaudio.h \
    $$PWD/groupbox_positionalaudio_servers.h \
    $$PWD/definitions_positionalaudio.h \
    $$PWD/groupbox_positionalaudio_status.h \
    $$PWD/guildwarstwo.h

SOURCES += \
    $$PWD/universe.cpp \
    $$PWD/mod_positionalaudio.cpp \
    $$PWD/settings_positionalaudio.cpp \
    $$PWD/groupbox_positional_audio.cpp \
    $$PWD/config_positionalaudio.cpp \
    $$PWD/groupbox_positionalaudio_servers.cpp \
    $$PWD/groupbox_positionalaudio_status.cpp \
    $$PWD/guildwarstwo.cpp

FORMS += \
    $$PWD/groupbox_positional_audio.ui \
    $$PWD/config_positionalaudio.ui \
    $$PWD/groupbox_positionalaudio_servers.ui \
    $$PWD/groupbox_positionalaudio_status.ui

beta {
    HEADERS +=
        $$PWD/gw.h
    SOURCES +=
        $$PWD/gw.cpp
}
