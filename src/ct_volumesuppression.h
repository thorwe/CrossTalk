#ifndef CT_VOLUMESUPPRESSION_H
#define CT_VOLUMESUPPRESSION_H

#include <QObject>
#include "public_definitions.h"
#include "tsfunctions.h"
#include "simple_volume.h"
#include "talkers.h"

class CT_VolumeSuppression : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool enabled
               READ isEnabled
               WRITE setEnabled
               NOTIFY enabledSet)
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

    bool isEnabled() const;
    bool isActive() {return m_isActive;}

    void setActive(bool); // for testing command, move to private later

private:
    bool m_enabled;
    bool m_isReverse;
    bool m_isActive;

    float value() {return m_value;}
    float m_value;

    uint64 m_homeId;

    TSFunctions *ts;
    Talkers* talkers;

    // Note: Client UI:(-30 to 20), while the API allows (-50.0 to +20.0)
//    QMap<anyID,SimpleVolume*>* ChannelVolumes;
//    void AddToChannelVolumes(uint64 serverConnectionHandlerID, anyID clientID); // Note that serverConnectionHandlerID is only there for checking if the client is a real client
//    void RemoveFromChannelVolumes(anyID clientID);
//    void ClearChannelVolumes();

    QMap<uint64, QMap<anyID,SimpleVolume*>* >* ServerChannelVolumes;
    void AddVolume(uint64 serverConnectionHandlerID, anyID clientID);
    void DeleteVolume(SimpleVolume *vol);
    void RemoveVolume(uint64 serverConnectionHandlerID, anyID clientID);
    void ClearServerChannelVolumes(uint64 serverConnectionHandlerID);
    void ClearServerChannelVolumes();

signals:
    void enabledSet(bool);
    void valueSet(float);
    void activeSet(bool);

public slots:
    void setEnabled(bool value);
    void setValue(float newValue);
    void setDuckingReverse(bool);

    void onConnectStatusChanged(uint64 serverConnectionHandlerID, int newStatus, unsigned int errorNumber);
    void onTalkStatusChanged(uint64 serverConnectionHandlerID, int status, bool isReceivedWhisper, anyID clientID);
};

#endif // CT_VOLUMESUPPRESSION_H
