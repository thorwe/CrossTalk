#ifndef DEFINITIONS_POSITIONALAUDIO_H
#define DEFINITIONS_POSITIONALAUDIO_H
#include <QString>
struct PositionalAudio_ServerSettings {
    QString serverUniqueId;
    QString serverName;
    bool enabled;
    bool isBlocked;
    float sendInterval;
    float sendIntervalSilentInc;
};

#endif // DEFINITIONS_POSITIONALAUDIO_H
