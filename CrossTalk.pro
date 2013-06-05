TEMPLATE = lib
# CONFIG += console
#CONFIG-=embed_manifest_dll

#VERSION = 1.2.3
CONFIG += beta

QT += sql network

INCLUDEPATH += includes \

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
    src/ts_settings_qt.h \
    src/ts_infodata_qt.h \
    src/ts_context_menu_qt.h \
    src/ts_logging_qt.h \
    src/ts_helpers_qt.h \
    src/ts_ptt_qt.h \
    src/updater.h \
    src/translator.h \
    src/banner.h \
    src/plugin.h \
    src/config.h \
    src/simple_volume.h \
    src/snt.h \
    src/talkers.h \
    src/simplepanner.h \
    src/module.h \
    src/volumes.h \
    src/mod_ducker_channel.h \
    src/mod_ducker_global.h \
    src/mod_muter_channel.h \
    src/mod_position_spread.h \
    src/config_ducking.h \
    src/fader_vertical.h \
    src/settings_duck.h \
    src/config_position_spread.h \
    src/settings_position_spread.h \
    src/banner_frame.h \
    src/ts_serverinfo_qt.h \
    src/ts_serversinfo.h \
    src/settings_radio.h

SOURCES += \
    src/ts_settings_qt.cpp \
    src/ts_infodata_qt.cpp \
    src/ts_context_menu_qt.cpp \
    src/ts_logging_qt.cpp \
    src/ts_helpers_qt.cpp \
    src/ts_ptt_qt.cpp \
    src/updater.cpp \
    src/translator.cpp \
    src/banner.cpp \
    src/plugin.cpp \
    src/config.cpp \
    src/simple_volume.cpp \
    src/snt.cpp \
    src/talkers.cpp \
    src/simplepanner.cpp \
    src/module.cpp \
    src/volumes.cpp \
    src/mod_ducker_channel.cpp \
    src/mod_ducker_global.cpp \
    src/mod_muter_channel.cpp \
    src/mod_position_spread.cpp \
    src/config_ducking.cpp \
    src/fader_vertical.cpp \
    src/settings_duck.cpp \
    src/config_position_spread.cpp \
    src/settings_position_spread.cpp \
    src/banner_frame.cpp \
    src/ts_serverinfo_qt.cpp \
    src/ts_serversinfo.cpp \
    src/settings_radio.cpp

FORMS += \
    src/config.ui \
    src/config_ducking.ui \
    src/fader_vertical.ui \
    src/config_position_spread.ui \
    src/banner_frame.ui

RESOURCES += \
    CrossTalkRes.qrc

beta {
    DEFINES += CROSSTALK_BETA

    # Positional Audio Module
    include(src/positional_audio/PositionalAudio.pri) {
        DEFINES += USE_POSITIONAL_AUDIO
        !build_pass:message( "Positional Audio module included." )
    }

    # Radio Module
    include(DSPFilters/DSPFilters.pri) {
        HEADERS += \
            src/dsp_radio.h \
            src/mod_radio.h \
            src/config_radio.h \
            src/config_radio_groupbox.h

        SOURCES += \
            src/dsp_radio.cpp \
            src/mod_radio.cpp \
            src/config_radio.cpp \
            src/config_radio_groupbox.cpp

        FORMS += \
            src/config_radio.ui \
            src/config_radio_groupbox.ui

        DEFINES += USE_RADIO
        !build_pass:message( "Radio module included." )
    }

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
