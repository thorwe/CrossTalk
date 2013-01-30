#ifndef CT_VOLUMESUPPRESSION_H
#define CT_VOLUMESUPPRESSION_H

#include <QObject>
#include "public_definitions.h"
#include "module.h"
#include "volumes.h"
#include "tsfunctions.h"
#include "simple_volume.h"
#include "talkers.h"

#include "ducker_global.h"

class CT_VolumeSuppression : public Module
{
    Q_OBJECT
    Q_PROPERTY(bool isActive
               READ isActive
               WRITE setActive
               NOTIFY activeSet)
    Q_PROPERTY(float value
               READ value
               WRITE setValue
               NOTIFY valueSet)
    Q_PROPERTY(uint64 homeId
               READ homeId
               WRITE setHomeId)

public:
    explicit CT_VolumeSuppression(QObject *parent = 0);

    // events forwarded from plugin.cpp
    void onClientMoveEvent(uint64 serverConnectionHandlerID, anyID clientID, uint64 oldChannelID, uint64 newChannelID, int visibility);
    void onEditPlaybackVoiceDataEvent(uint64 serverConnectionHandlerID, anyID clientID, short* samples, int sampleCount, int channels);

    void setHomeId(uint64 serverConnectionHandlerID);
    uint64 homeId() {return m_homeId;}
    bool isActive() {return m_isActive;}
    void setActive(bool); // for testing command, move to private later

    Ducker_Global* ducker_G; // until I figure out why the class behaves weird as singleton

private:
    bool m_isReverse;
    bool m_isActive;
    float value() {return m_value;}
    float m_value;
    uint64 m_homeId;

    TSFunctions *ts;
    Talkers* talkers;
    Volumes* vols;

    SimpleVolume *AddDuckerVolume(uint64 serverConnectionHandlerID, anyID clientID);

signals:
    void valueSet(float);
    void activeSet(bool);

public slots:
    void setValue(float newValue);
    void setDuckingReverse(bool);

    void onTalkStatusChanged(uint64 serverConnectionHandlerID, int status, bool isReceivedWhisper, anyID clientID);

protected:
    void onRunningStateChanged(bool value);
};

#endif // CT_VOLUMESUPPRESSION_H
