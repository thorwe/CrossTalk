INCLUDEPATH += $$PWD \
    $$PWD/..

include(../../DSPFilters/DSPFilters.pri) {
    HEADERS += \
        $$PWD/dsp_radio.h \
        $$PWD/mod_radio.h \
        $$PWD/config_radio.h \
        $$PWD/config_radio_groupbox.h \
        $$PWD/settings_radio.h

    SOURCES += \
        $$PWD/dsp_radio.cpp \
        $$PWD/mod_radio.cpp \
        $$PWD/config_radio.cpp \
        $$PWD/config_radio_groupbox.cpp \
        $$PWD/settings_radio.cpp

    FORMS += \
        $$PWD/config_radio.ui \
        $$PWD/config_radio_groupbox.ui
}
