#pragma once

#include <QObject>
#include "teamspeak/public_definitions.h"
#include "module.h"
#include "volumes.h"
#include "dsp_volume_ducker.h"
#include "talkers.h"

class Ducker_Channel : public Module, public TalkInterface
{
    Q_OBJECT
    Q_INTERFACES(TalkInterface)
    Q_PROPERTY(bool isActive
               READ isActive
               WRITE setActive
               NOTIFY activeSet)
    Q_PROPERTY(float value
               READ getValue
               WRITE setValue
               NOTIFY valueSet)
    Q_PROPERTY(uint64 homeId
               READ homeId
               WRITE setHomeId)
    Q_PROPERTY(bool targetOtherTabs
               READ isTargetOtherTabs)
    Q_PROPERTY(bool duckPrioritySpeakers
               READ isDuckPrioritySpeakers)

public:
    explicit Ducker_Channel(QObject *parent = 0);

    float getValue() const;
    bool isTargetOtherTabs() const;
    bool isDuckPrioritySpeakers() const;

    // events forwarded from plugin.cpp
    void onClientMoveEvent(uint64 serverConnectionHandlerID, anyID clientID, uint64 oldChannelID, uint64 newChannelID, int visibility, anyID myID);
    void onEditPlaybackVoiceDataEvent(uint64 serverConnectionHandlerID, anyID clientID, short* samples, int sampleCount, int channels);

    void setHomeId(uint64 serverConnectionHandlerID);
    uint64 homeId() {return m_homeId;}
    bool isActive() {return m_isActive;}
    void setActive(bool); // for testing command, move to private later

    bool onTalkStatusChanged(uint64 serverConnectionHandlerID, int status, bool isReceivedWhisper, anyID clientID, bool isMe);

private:
    bool m_isTargetOtherTabs = false;
    bool m_isActive = false;
    float value() {return m_value;}
    float m_value = 0.0f;
    uint64 m_homeId = 0;
    bool m_isDuckPrioritySpeakers = false;

    Talkers* talkers;
    Volumes* vols;

    DspVolumeDucker* AddDuckerVolume(uint64 serverConnectionHandlerID, anyID clientID);
    void UpdateActive();

signals:
    void valueSet(float);
    void activeSet(bool);

public slots:
    void setValue(float newValue);
    void setDuckingReverse(bool);
    void setDuckPrioritySpeakers(bool);

//    void saveSettings();

protected:
    void onRunningStateChanged(bool value);
};
