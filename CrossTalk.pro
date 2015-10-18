TEMPLATE = lib
# CONFIG += console
#CONFIG-=embed_manifest_dll

#VERSION = 1.3.2
#CONFIG += beta

QT += sql network widgets

INCLUDEPATH += includes

TRANSLATIONS = crosstalk_de_DE.ts

HEADERS += \
    includes/clientlib_publicdefinitions.h \
    includes/ts3_functions.h \
    includes/public_rare_definitions.h \
    includes/public_errors_rare.h \
    includes/public_errors.h \
    includes/public_definitions.h \
    includes/plugin_definitions.h \
    includes/db.h \
    includes/MMtoDB.h \
    includes/ts_missing_definitions.h \
    includes/dsp_helpers.h \
    src/ts_settings_qt.h \
    src/ts_infodata_qt.h \
    src/ts_context_menu_qt.h \
    src/ts_logging_qt.h \
    src/ts_helpers_qt.h \
    src/ts_ptt_qt.h \
    src/ts_serverinfo_qt.h \
    src/ts_serversinfo.h \
    src/updater.h \
    src/translator.h \
    src/banner.h \
    src/plugin.h \
    src/config.h \
    src/snt.h \
    src/talkers.h \
    src/simplepanner.h \
    src/module.h \
    src/dsp_volume.h \
    src/dsp_volume_ducker.h \
    src/dsp_volume_agmu.h \
    src/volumes.h \
    src/mod_ducker_channel.h \
    src/mod_ducker_global.h \
    src/mod_muter_channel.h \
    src/mod_position_spread.h \
    src/config_ducking.h \
    src/fader_vertical.h \
    src/settings_duck.h \
    src/groupbox_position_spread.h \
    src/settings_position_spread.h \
    src/banner_frame.h \
    src/mod_agmu.h \
    src/plugin_qt.h \
    src/sse_server.h \
    src/groupbox_ducking.h \
    src/pipeserver.h \
    src/version_qt.h

SOURCES += \
    src/ts_settings_qt.cpp \
    src/ts_infodata_qt.cpp \
    src/ts_context_menu_qt.cpp \
    src/ts_logging_qt.cpp \
    src/ts_helpers_qt.cpp \
    src/ts_ptt_qt.cpp \
    src/ts_serverinfo_qt.cpp \
    src/ts_serversinfo.cpp \
    src/updater.cpp \
    src/translator.cpp \
    src/banner.cpp \
    src/plugin.cpp \
    src/config.cpp \
    src/snt.cpp \
    src/talkers.cpp \
    src/simplepanner.cpp \
    src/module.cpp  \
    src/dsp_volume.cpp \
    src/dsp_volume_ducker.cpp \
    src/dsp_volume_agmu.cpp \
    src/volumes.cpp \
    src/mod_ducker_channel.cpp \
    src/mod_ducker_global.cpp \
    src/mod_muter_channel.cpp \
    src/mod_position_spread.cpp \
    src/config_ducking.cpp \
    src/fader_vertical.cpp \
    src/settings_duck.cpp \
    src/groupbox_position_spread.cpp \
    src/settings_position_spread.cpp \
    src/banner_frame.cpp \
    src/mod_agmu.cpp \
    src/plugin_qt.cpp \
    src/sse_server.cpp \
    src/groupbox_ducking.cpp \
    src/pipeserver.cpp \
    src/version_qt.cpp

FORMS += \
    src/config.ui \
    src/config_ducking.ui \
    src/fader_vertical.ui \
    src/groupbox_position_spread.ui \
    src/banner_frame.ui \
    src/groupbox_ducking.ui

RESOURCES += \
    CrossTalkRes.qrc

# Radio Module
include(src/radio/Radio.pri) {
    DEFINES += USE_RADIO
    !build_pass:message( "Radio module included." )
}

# Positional Audio Module
include(src/positional_audio/PositionalAudio.pri) {
    DEFINES += USE_POSITIONAL_AUDIO
    !build_pass:message( "Positional Audio module included." )
}

#greaterThan(QT_MAJOR_VERSION, 4): include(QtWebApp/QtWebApp.pri) {
#    DEFINES += USE_QT_WEB_APP
#    !build_pass:message( "QtWebApp included." )
#}
include (QtWebsocket/QtWebsocket.pri) {
    DEFINES += USE_WEBSOCKET
    !build_pass:message( "Websockets included." )
}

#DEFINES += CT_VERBOSE
#DEFINES += CONSOLE_OUTPUT
beta {
    DEFINES += CROSSTALK_BETA
    !build_pass:message( "Beta." )
}

win32 {
    CONFIG(release, debug|release){ #release build
        QMAKE_CXXFLAGS_RELEASE += -Zi
        QMAKE_CFLAGS_RELEASE += -Zi
        QMAKE_LFLAGS_RELEASE +=  -DEBUG -OPT:REF #/DEBUG /OPT:REF
    }
    else {  #not used atm
        QMAKE_CXXFLAGS_RELEASE += -Zi
        QMAKE_CFLAGS_RELEASE += -Zi
        QMAKE_LFLAGS_RELEASE += -DEBUG -OPT:REF

        QMAKE_CFLAGS_DEBUG = -O2 -MD -Zi -MP2 #$$QMAKE_CXXFLAGS_RELEASE_WITH_DEBUGINFO
        QMAKE_CXXFLAGS_DEBUG = -O2 -MD -Zi -MP2 #$$QMAKE_CXXFLAGS_RELEASE_WITH_DEBUGINFO
        QMAKE_LFLAGS_DEBUG = /DEBUG /OPT:REF #$$QMAKE_LFLAGS_RELEASE
    }
}
