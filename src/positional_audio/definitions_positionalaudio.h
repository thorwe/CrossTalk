#pragma once

#include <QtCore/QString>
struct PositionalAudio_ServerSettings {
    QString serverUniqueId;
    QString serverName;
    bool enabled;
    bool isBlocked;
    float sendInterval;
    float sendIntervalSilentInc;
};
