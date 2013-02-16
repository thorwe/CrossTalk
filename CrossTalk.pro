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
    src/ct_volumesuppression.h \
    src/tsfunctions.h \
    src/ts_settings_qt.h \
    src/simple_volume.h \
    src/snt.h \
    src/pantalkers.h \
    src/talkers.h \
    src/simplepanner.h \
    src/ducker_global.h \
    src/module.h \
    src/volumes.h \
    src/muter_channel.h \
    src/ts_logging_qt.h \
    src/ts_helpers_qt.h \
    src/ts_ptt_qt.h

SOURCES += \
    src/banner.cpp \
    src/plugin.cpp \
    src/config.cpp \
    src/ct_volumesuppression.cpp \
    src/tsfunctions.cpp \
    src/ts_settings_qt.cpp \
    src/simple_volume.cpp \
    src/snt.cpp \
    src/pantalkers.cpp \
    src/talkers.cpp \
    src/simplepanner.cpp \
    src/ducker_global.cpp \
    src/module.cpp \
    src/volumes.cpp \
    src/muter_channel.cpp \
    src/ts_logging_qt.cpp \
    src/ts_helpers_qt.cpp \
    src/ts_ptt_qt.cpp

FORMS += \
    src/config.ui

RESOURCES += \
    CrossTalkRes.qrc
