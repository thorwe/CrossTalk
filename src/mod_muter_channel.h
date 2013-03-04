#ifndef MOD_MUTER_CHANNEL_H
#define MOD_MUTER_CHANNEL_H

#include <QObject>
#include "public_definitions.h"
#include "module.h"
#include "volumes.h"
#include "simple_volume.h"
#include "talkers.h"

class ChannelMuter : public Module
{
    Q_OBJECT
protected:
    void onRunningStateChanged(bool value);
public:
    explicit ChannelMuter(QObject *parent = 0);

    // user interaction
    bool toggleChannelMute(uint64 serverConnectionHandlerID,uint64 channelID);
    bool isChannelMuted(uint64 serverConnectionHandlerID, uint64 channelID);
    bool toggleClientWhitelisted(uint64 serverConnectionHandlerID, anyID clientID);
    bool isClientWhitelisted(uint64 serverConnectionHandlerID, anyID clientID);

    // events forwarded from plugin.cpp
    void onClientMoveEvent(uint64 serverConnectionHandlerID, anyID clientID, uint64 oldChannelID, uint64 newChannelID, int visibility, anyID myID);
    bool onEditPlaybackVoiceDataEvent(uint64 serverConnectionHandlerID, anyID clientID, short* samples, int sampleCount, int channels);

    bool onTalkStatusChanged(uint64 serverConnectionHandlerID, int status, bool isReceivedWhisper, anyID clientID, bool isMe);
signals:
    
public slots:

private:
    Talkers* talkers;
    Volumes* vols;

    QSet<QPair<uint64,uint64> >* MutedChannels;
    QSet<QPair<uint64,anyID> >* ClientWhiteList;
};

#endif // MOD_MUTER_CHANNEL_H
