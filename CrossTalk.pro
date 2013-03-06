TEMPLATE = lib
# CONFIG += console
#CONFIG-=embed_manifest_dll

QT += sql += network

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
    src/ts_logging_qt.h \
    src/ts_helpers_qt.h \
    src/ts_ptt_qt.h \
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
    src/translator.h

SOURCES += \
    src/ts_settings_qt.cpp \
    src/ts_infodata_qt.cpp \
    src/ts_logging_qt.cpp \
    src/ts_helpers_qt.cpp \
    src/ts_ptt_qt.cpp \
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
    src/translator.cpp

FORMS += \
    src/config.ui

RESOURCES += \
    CrossTalkRes.qrc

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
