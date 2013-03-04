TEMPLATE = lib
# CONFIG += console

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
    src/banner.h \
    src/plugin.h \
    src/config.h \
    src/ts_settings_qt.h \
    src/simple_volume.h \
    src/snt.h \
    src/talkers.h \
    src/simplepanner.h \
    src/module.h \
    src/volumes.h \
    src/ts_logging_qt.h \
    src/ts_helpers_qt.h \
    src/ts_ptt_qt.h \
    src/mod_ducker_channel.h \
    src/mod_ducker_global.h \
    src/mod_muter_channel.h \
    src/mod_position_spread.h \
    src/translator.h #\
#    src/mod_positionalaudio.h \
#    src/universe.h #\
#    src/ts_infodata_qt.h

SOURCES += \
    src/banner.cpp \
    src/plugin.cpp \
    src/config.cpp \
    src/ts_settings_qt.cpp \
    src/simple_volume.cpp \
    src/snt.cpp \
    src/talkers.cpp \
    src/simplepanner.cpp \
    src/module.cpp \
    src/volumes.cpp \
    src/ts_logging_qt.cpp \
    src/ts_helpers_qt.cpp \
    src/ts_ptt_qt.cpp \
    src/mod_ducker_channel.cpp \
    src/mod_ducker_global.cpp \
    src/mod_muter_channel.cpp \
    src/mod_position_spread.cpp \
    src/translator.cpp \
#    src/mod_positionalaudio.cpp \
#    src/universe.cpp \
#    src/ts_infodata_qt.cpp

FORMS += \
    src/config.ui

RESOURCES += \
    CrossTalkRes.qrc
