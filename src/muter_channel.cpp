#include "muter_channel.h"

#include "public_errors.h"
#include "public_errors_rare.h"
#include "public_definitions.h"
#include "public_rare_definitions.h"
#include "ts3_functions.h"

#include "plugin.h"
#include "tsfunctions.h"

#include "ts_helpers_qt.h"

ChannelMuter::ChannelMuter(QObject *parent)
{
    m_isPrintEnabled = false;
    this->setParent(parent);
    this->setObjectName("ChannelMuter");
    ts = TSFunctions::instance();
    talkers = Talkers::instance();
    vols = new Volumes(this);
    MutedChannels = new QSet<QPair<uint64,uint64> >;
    ClientWhiteList = new QSet<QPair<uint64,anyID> >;
}

// User Setting interaction

void ChannelMuter::onRunningStateChanged(bool value)
{
    if (!value)  // this module is always active, used solely as init function
        return;

    connect(talkers,SIGNAL(TalkStatusChanged(uint64,int,bool,anyID)),this,SLOT(onTalkStatusChanged(uint64,int,bool,anyID)));
    connect(talkers,SIGNAL(ConnectStatusChanged(uint64,int,uint)),vols,SLOT(onConnectStatusChanged(uint64,int,uint)));

    Log(QString("enabled: %1").arg((value)?"true":"false"));
}

//! Toggles a channels mute status on a server tab
/*!
 * \brief ChannelMuter::toggleChannelMute Toggles a channels mute status on a server tab
 * \param serverConnectionHandlerID the server tab
 * \param channelID the channel id
 * \return true if the toggling results in muting, otherwise false
 */
bool ChannelMuter::toggleChannelMute(uint64 serverConnectionHandlerID, uint64 channelID)
{
    QPair<uint64,uint64> newPair = qMakePair(serverConnectionHandlerID,channelID);
    if (!(MutedChannels->contains(newPair)))
        MutedChannels->insert(newPair);
    else
        MutedChannels->remove(newPair);

    unsigned int error;
    // Get My Id on this handler
    anyID myID;
    if((error = ts3Functions.getClientID(serverConnectionHandlerID,&myID)) != ERROR_ok)
        Error("(toggleChannelMute) Error getting my client id",serverConnectionHandlerID,error);
    else
    {
        // Get My channel on this handler
        uint64 myChannelID;
        if((error = ts3Functions.getChannelOfClient(serverConnectionHandlerID,myID,&myChannelID)) != ERROR_ok)
            Error("(toggleChannelMute) Error getting Client Channel Id",serverConnectionHandlerID,error);
        else
        {
            if (channelID == myChannelID)   // only if it's my current channel immediate action is necessary
            {
                // Get Channel Client List
                anyID* clients;
                if((error = ts3Functions.getChannelClientList(serverConnectionHandlerID,channelID, &clients)) != ERROR_ok)
                    Error("(toggleChannelMute) Error getting Client Channel List",serverConnectionHandlerID,error);
                else
                {
                    for(int i=0; clients[i]; i++)   // Iterate and push fake onTalkStatusChanged events to the module
                    {
                        if (clients[i] == myID)
                            continue;

                        int talkStatus;
                        int isReceivedWhisper;
                        if ((error = TSHelpers::GetTalkStatus(serverConnectionHandlerID,clients[i],talkStatus,isReceivedWhisper)) != ERROR_ok)
                            Error("(toggleChannelMute) Error receiving client talk status",serverConnectionHandlerID,error);
                        else
                            onTalkStatusChanged(serverConnectionHandlerID, talkStatus, (isReceivedWhisper == 1), clients[i]);
                    }
                }
            }
        }
    }

    return (MutedChannels->contains(newPair));
}

//! Is a channel muted on the server tab?
/*!
 * \brief ChannelMuter::isChannelMuted Is a channel muted on the server tab?
 * \param serverConnectionHandlerID the server tab
 * \param channelID the channel id
 * \return true if muted
 */
bool ChannelMuter::isChannelMuted(uint64 serverConnectionHandlerID, uint64 channelID)
{
    QPair<uint64,uint64> newPair = qMakePair(serverConnectionHandlerID,channelID);
    return (MutedChannels->contains(newPair));
}

//! toggles a client whitelist status, receiver of some user interaction
/*!
 * \brief ChannelMuter::toggleClientWhitelisted toggles a client whitelist status, receiver of some user interaction
 * \param serverConnectionHandlerID ther server tab
 * \param clientID the client id
 * \return true if the action results in whitelisting, otherwise false
 */
bool ChannelMuter::toggleClientWhitelisted(uint64 serverConnectionHandlerID, anyID clientID)
{
    QPair<uint64,anyID> newPair = qMakePair(serverConnectionHandlerID,clientID);
    if (!(ClientWhiteList->contains(newPair)))
        ClientWhiteList->insert(newPair);
    else
        ClientWhiteList->remove(newPair);

    int talkStatus;
    int isReceivedWhisper;
    unsigned int error;
    if ((error = TSHelpers::GetTalkStatus(serverConnectionHandlerID,clientID,talkStatus,isReceivedWhisper)) != ERROR_ok)
        Error("(toggleClientWhitelisted) Error receiving client talk status",serverConnectionHandlerID,error);
    else
        onTalkStatusChanged(serverConnectionHandlerID, talkStatus, (isReceivedWhisper == 1), clientID);

    return (ClientWhiteList->contains(newPair));
}

//! Is a client whitelisted on a serverConnectionHandlerID?
/*!
 * \brief ChannelMuter::isClientWhitelisted Is a client whitelisted on a serverConnectionHandlerID?
 * \param serverConnectionHandlerID the server tab
 * \param clientID the client id
 * \return true if whitelisted
 */
bool ChannelMuter::isClientWhitelisted(uint64 serverConnectionHandlerID, anyID clientID)
{
    QPair<uint64,anyID> newPair = qMakePair(serverConnectionHandlerID,clientID);
    return (ClientWhiteList->contains(newPair));
}

// ts event handlers

//! Book keeping of volume objects when clients switch channels etc.
/*!
 * \brief ChannelMuter::onClientMoveEvent Book keeping of volume objects when clients switch channels etc.
 * \param serverConnectionHandlerID the server tab
 * \param clientID the client
 * \param oldChannelID the...old channel
 * \param newChannelID the...new channel!
 * \param visibility unused here
 */
void ChannelMuter::onClientMoveEvent(uint64 serverConnectionHandlerID, anyID clientID, uint64 oldChannelID, uint64 newChannelID, int visibility)
{
    Q_UNUSED(visibility);

    // Get My Id on this handler
    anyID myID;
    if(ts->GetClientId(serverConnectionHandlerID,&myID) != ERROR_ok)
        return;

    if (clientID == myID)                   // I have switched channels
    {
        if (newChannelID == 0)
            return;

        vols->RemoveVolumes(serverConnectionHandlerID);

        unsigned int error;
        // Get Channel Client List
        anyID* clients;
        if((error = ts3Functions.getChannelClientList(serverConnectionHandlerID, newChannelID, &clients)) != ERROR_ok)
            Error("(onClientMoveEvent): Error getting Channel Client List", serverConnectionHandlerID, error);
        else
        {
            // for every client insert volume
            for(int i=0; clients[i]; i++)
            {
                if (clients[i] == myID)
                    continue;

                vols->AddVolume(serverConnectionHandlerID,clients[i]);
            }
        }
    }
    else                                    // Someone else has...
    {
        // Get My channel on this handler
        uint64 channelID;
        unsigned int error;
        if((error = ts3Functions.getChannelOfClient(serverConnectionHandlerID,myID,&channelID)) != ERROR_ok)
            Error("(onClientMoveEvent) Error getting Client Channel Id",serverConnectionHandlerID,error);
        else
        {
            if (channelID == oldChannelID)      // left
                vols->RemoveVolume(serverConnectionHandlerID,clientID);
            else if (channelID == newChannelID) // joined
                vols->AddVolume(serverConnectionHandlerID,clientID);
        }
    }
}

//! Set the status of volume object based on the talk status
/*!
 * \brief ChannelMuter::onTalkStatusChanged Set the status of volume object based on the talk status
 * \param serverConnectionHandlerID the server tab
 * \param status -> enum TalkStatus
 * \param isReceivedWhisper is it a whisper?
 * \param clientID the client changing its talk status
 */
void ChannelMuter::onTalkStatusChanged(uint64 serverConnectionHandlerID, int status, bool isReceivedWhisper, anyID clientID)
{
    //Print("(onTalkStatusChanged)", serverConnectionHandlerID, LogLevel_INFO);

    if (isReceivedWhisper)
        return;

    anyID myID;
    if(ts->GetClientId(serverConnectionHandlerID,&myID) != ERROR_ok)
        return;

    if (clientID == myID)
        return;

    if (((status==STATUS_TALKING) || (status==STATUS_NOT_TALKING)) && (vols->VolumesMap->contains(serverConnectionHandlerID)))
    {
        QMap<anyID,SimpleVolume*>* ChanVolumes = vols->VolumesMap->value(serverConnectionHandlerID);
        if (ChanVolumes->contains(clientID))
        {
            unsigned int error = ERROR_ok;
            uint64 channelID;
            if((error = ts3Functions.getChannelOfClient(serverConnectionHandlerID,clientID,&channelID)) != ERROR_ok)
                Error("(onTalkStatusChanged) Error getting Client Channel Id",serverConnectionHandlerID,error);
            else
            {
                SimpleVolume* vol = ChanVolumes->value(clientID);
                QPair<uint64,uint64> channelPair = qMakePair(serverConnectionHandlerID,channelID);
                QPair<uint64,anyID> clientPair = qMakePair(serverConnectionHandlerID,clientID);
                vol->setMuted((MutedChannels->contains(channelPair)) && (!(ClientWhiteList->contains(clientPair))));
                vol->setProcessing(status==STATUS_TALKING);
            }
        }
    }
}

//! Routes the arguments of the event to the corresponding volume object
/*!
 * \brief ChannelMuter::onEditPlaybackVoiceDataEvent pre-processing voice event
 * \param serverConnectionHandlerID the connection id of the server
 * \param clientID the client-side runtime-id of the sender
 * \param samples the sample array to manipulate
 * \param sampleCount amount of samples in the array
 * \param channels amount of channels
 * \return true if the stream is muted, so succeeding modules need not bother
 */
bool ChannelMuter::onEditPlaybackVoiceDataEvent(uint64 serverConnectionHandlerID, anyID clientID, short *samples, int sampleCount, int channels)
{
    if (!(isRunning()))
        return false;

    if (!vols->VolumesMap->contains(serverConnectionHandlerID))
        return false;

    QMap<anyID,SimpleVolume*>* ChanVolumes = vols->VolumesMap->value(serverConnectionHandlerID);
    if (!ChanVolumes->contains(clientID))
        return false;

    sampleCount = sampleCount * channels;
    SimpleVolume* vol = ChanVolumes->value(clientID);
    vol->process(samples,sampleCount);
    return vol->isMuted();
}
