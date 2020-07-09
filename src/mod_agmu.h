#pragma once

#include "volume/dsp_volume_agmu.h"
#include "volume/volumes.h"

#include "core/module_qt.h"
#include "core/plugin_base.h"

#include <map>

class Agmu : public Module_Qt, public TalkInterface
{
    Q_OBJECT
    Q_INTERFACES(TalkInterface)

public:
    Agmu(Plugin_Base& plugin);
    
    // events forwarded from plugin.cpp
    bool onEditPlaybackVoiceDataEvent(uint64 serverConnectionHandlerID, anyID clientID, short* samples, int sampleCount, int channels);
    bool onTalkStatusChanged(uint64 serverConnectionHandlerID, int status, bool isReceivedWhisper, anyID clientID, bool isMe);
    void setNextTalkStatusChangeForceProcessing(bool val);  // well, yeah...don't wanna change the interface right now

private:
    void onRunningStateChanged(bool value);

    Talkers &m_talkers;

    thorwe::volume::Volumes<DspVolumeAGMU> m_volumes;

    // TODO not thread safe
    std::map<std::string, int16_t> m_peak_cache;
    bool m_is_force_processing = false;
};
