#pragma once

#include "teamspeak/public_definitions.h"

#include <QtCore/QString>
struct PositionalAudio_ServerSettings {
    QString serverUniqueId;
    QString serverName;
    bool enabled;
    bool isBlocked;
    float sendInterval;
    float sendIntervalSilentInc;
};

namespace thorwe
{
struct Three_Dee_Info
{
    TS3_VECTOR position = {0.f, 0.f, 0.f};
    TS3_VECTOR front = {0.f, 0.f, 0.f};
    TS3_VECTOR top = {0.f, 0.f, 0.f};
};
}  // namespace thorwe
