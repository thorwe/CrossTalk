#ifndef MUTER_CHANNEL_H
#define MUTER_CHANNEL_H

#include <QObject>
#include "public_definitions.h"
#include "module.h"
#include "volumes.h"
#include "tsfunctions.h"
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
    void onClientMoveEvent(uint64 serverConnectionHandlerID, anyID clientID, uint64 oldChannelID, uint64 newChannelID, int visibility);
    bool onEditPlaybackVoiceDataEvent(uint64 serverConnectionHandlerID, anyID clientID, short* samples, int sampleCount, int channels);

signals:
    
public slots:
   void onTalkStatusChanged(uint64 serverConnectionHandlerID, int status, bool isReceivedWhisper, anyID clientID);

private:
    TSFunctions *ts;
    Talkers* talkers;
    Volumes* vols;
    //QMap<uint64,QSet<uint64>* >* MutedChannels;
    QSet<QPair<uint64,uint64> >* MutedChannels;
    QSet<QPair<uint64,anyID> >* ClientWhiteList;
};

#endif // MUTER_CHANNEL_H
