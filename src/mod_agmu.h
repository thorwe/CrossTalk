#pragma once

#include "core/module.h"
#include "volume/dsp_volume_agmu.h"
#include "core/plugin_base.h"

class Agmu : public Module, public TalkInterface
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

    Talkers& m_talkers;

    QMap<uint64,QMap<anyID,DspVolumeAGMU*>* >* m_TalkersDSPs;   //QMap is reportedly faster on small (<10)
    QHash<QString,short>* m_PeakCache;
    bool m_isForceProcessing = false;
};
