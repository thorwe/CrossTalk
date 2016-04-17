#include "mod_muter_channel.h"

#include "teamspeak/public_errors.h"
#include "teamspeak/public_errors_rare.h"
#include "teamspeak/public_definitions.h"
#include "teamspeak/public_rare_definitions.h"
#include "ts3_functions.h"

#include "plugin.h"

#include "ts_helpers_qt.h"

#include "talkers.h"

ChannelMuter::ChannelMuter(QObject *parent)
{
    m_isPrintEnabled = false;
    this->setParent(parent);
    this->setObjectName("ChannelMuter");
    vols = new Volumes(this,VolumeTypeManual);
//    MutedChannels = new QSet<QPair<uint64,uint64> >;
//    ClientWhiteList = new QSet<QPair<uint64,anyID> >;
}

// User Setting interaction

void ChannelMuter::onRunningStateChanged(bool value)
{
    if (!value)  // this module is always active, used solely as init function
        return;

    if (m_ContextMenuIdToggleChannelMute == -1)
    {
        m_ContextMenuIdToggleChannelMute = TSContextMenu::instance()->Register(this,PLUGIN_MENU_TYPE_CHANNEL,"Toggle Channel Mute [temp]","");
        connect(TSContextMenu::instance(), &TSContextMenu::FireContextMenuEvent, this, &ChannelMuter::onContextMenuEvent, Qt::AutoConnection);
    }

    if (m_ContextMenuToggleClientWhitelisted == -1)
        m_ContextMenuToggleClientWhitelisted = TSContextMenu::instance()->Register(this,PLUGIN_MENU_TYPE_CLIENT,"Toggle ChannelMuter Whitelisting [temp]","");

    TSInfoData::instance()->Register(this,value,1);
    connect(Talkers::instance(), &Talkers::ConnectStatusChanged, vols, &Volumes::onConnectStatusChanged, Qt::UniqueConnection);

    Log(QString("enabled: %1").arg((value)?"true":"false"));
}

//! Toggles a channels mute status on a server tab
/*!
 * \brief ChannelMuter::toggleChannelMute Toggles a channels mute status on a server tab
 * \param serverConnectionHandlerID the server tab
 * \param channelID the channel id
 * \return true if the toggling results in active muting, otherwise false
 */
bool ChannelMuter::toggleChannelMute(uint64 serverConnectionHandlerID, uint64 channelID)
{

    if (serverConnectionHandlerID == (uint64)NULL)
    {
        serverConnectionHandlerID = TSHelpers::GetActiveServerConnectionHandlerID();
        if (serverConnectionHandlerID == (uint64)NULL)
            return false;
    }

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
            uint64 targetChannelId = (channelID != (uint64)NULL)?channelID:myChannelID;

            auto newPair = qMakePair(serverConnectionHandlerID,targetChannelId);
            if (!(MutedChannels.contains(newPair)))
                MutedChannels.insert(newPair);
            else
                MutedChannels.remove(newPair);

            if (targetChannelId == myChannelID)   // only if it's my current channel / hotkey immediate action is necessary
            {
                // Get Channel Client List
                anyID* clients;
                if((error = ts3Functions.getChannelClientList(serverConnectionHandlerID, targetChannelId, &clients)) != ERROR_ok)
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
                            onTalkStatusChanged(serverConnectionHandlerID, talkStatus, (isReceivedWhisper == 1), clients[i], clients[i]==myID);
                    }

                    // for info update on hotkey
                    if (channelID == (uint64)NULL)
                        TSInfoData::instance()->RequestUpdate(serverConnectionHandlerID, targetChannelId, PLUGIN_CHANNEL);
                }
            }
            return (MutedChannels.contains(newPair));
        }
    }
    return false;
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
    auto newPair = qMakePair(serverConnectionHandlerID,channelID);
    return (MutedChannels.contains(newPair));
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
    //Log(QString("(toggleClientWhitelisted) %1").arg(clientID),serverConnectionHandlerID,LogLevel_DEBUG);
    QPair<uint64,anyID> newPair = qMakePair(serverConnectionHandlerID,clientID);
    if (!(ClientWhiteList.contains(newPair)))
        ClientWhiteList.insert(newPair);
    else
        ClientWhiteList.remove(newPair);

    int talkStatus;
    int isReceivedWhisper;
    unsigned int error;
    if ((error = TSHelpers::GetTalkStatus(serverConnectionHandlerID,clientID,talkStatus,isReceivedWhisper)) != ERROR_ok)
        Error("(toggleClientWhitelisted) Error receiving client talk status",serverConnectionHandlerID,error);
    else
        onTalkStatusChanged(serverConnectionHandlerID, talkStatus, (isReceivedWhisper == 1), clientID,false);

    return (ClientWhiteList.contains(newPair));
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
    auto newPair = qMakePair(serverConnectionHandlerID,clientID);
    return (ClientWhiteList.contains(newPair));
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
void ChannelMuter::onClientMoveEvent(uint64 serverConnectionHandlerID, anyID clientID, uint64 oldChannelID, uint64 newChannelID, int visibility, anyID myID)
{
    Q_UNUSED(visibility);

    unsigned int error;
    if (clientID == myID)                   // I have switched channels
    {
        vols->RemoveVolumes(serverConnectionHandlerID);

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
        if((error = ts3Functions.getChannelOfClient(serverConnectionHandlerID,myID,&channelID)) != ERROR_ok)
            Error("(onClientMoveEvent) Error getting Client Channel Id",serverConnectionHandlerID,error);
        else
        {
            if (channelID == oldChannelID)      // left
                vols->RemoveVolume(serverConnectionHandlerID,clientID);
            else if (channelID == newChannelID) // joined
            {
                vols->AddVolume(serverConnectionHandlerID,clientID);
            }
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
bool ChannelMuter::onTalkStatusChanged(uint64 serverConnectionHandlerID, int status, bool isReceivedWhisper, anyID clientID, bool isMe)
{
    if (!isRunning())
        return false;

    if (isReceivedWhisper)
        return false;

    if (isMe)
        return false;

    if ((status==STATUS_TALKING) || (status==STATUS_NOT_TALKING))
    {
        auto vol = vols->GetVolume(serverConnectionHandlerID,clientID);
        if (!vol)
            return false;

        unsigned int error = ERROR_ok;
        uint64 channelID;
        if((error = ts3Functions.getChannelOfClient(serverConnectionHandlerID,clientID,&channelID)) != ERROR_ok)
        {
            if (error!=ERROR_not_connected)
                Error("(onTalkStatusChanged) Error getting Client Channel Id",serverConnectionHandlerID,error);
        }
        else
        {
            auto channelPair = qMakePair(serverConnectionHandlerID,channelID);
            auto clientPair = qMakePair(serverConnectionHandlerID,clientID);
            vol->setMuted((MutedChannels.contains(channelPair)) && (!(ClientWhiteList.contains(clientPair))));
            vol->setProcessing(status==STATUS_TALKING);
            return vol->isMuted();
        }
    }
    return false;
}

bool ChannelMuter::onInfoDataChanged(uint64 serverConnectionHandlerID, uint64 id, PluginItemType type, uint64 mine, QTextStream &data)
{
    auto isDirty = false;
    if (type == PLUGIN_CLIENT)
    {
        if (m_ContextMenuToggleClientWhitelisted != -1)
            ts3Functions.setPluginMenuEnabled(pluginID,m_ContextMenuToggleClientWhitelisted,(id != mine)?1:0);

        if ((id != mine) && isClientWhitelisted(serverConnectionHandlerID,(anyID)id))
        {
            data << this->objectName() << ":";
            isDirty = true;
            data << "whitelisted [temp]";
        }
    }
    else if (type == PLUGIN_CHANNEL)
    {
        if (isChannelMuted(serverConnectionHandlerID,id))
        {
            data << this->objectName() << ":";
            isDirty = true;
            data << "channel muted [temp]";
        }
    }
    return isDirty;
}

void ChannelMuter::onContextMenuEvent(uint64 serverConnectionHandlerID, PluginMenuType type, int menuItemID, uint64 selectedItemID)
{
    Q_UNUSED(type);
    if (menuItemID == m_ContextMenuIdToggleChannelMute)
        toggleChannelMute(serverConnectionHandlerID,selectedItemID);
    else if (menuItemID == m_ContextMenuToggleClientWhitelisted)
        toggleClientWhitelisted(serverConnectionHandlerID,(anyID)selectedItemID);
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

    auto vol = vols->GetVolume(serverConnectionHandlerID,clientID);
    if (!vol)
        return false;

    vol->process(samples, sampleCount, channels);
    return vol->isMuted();
}

int ChannelMuter::ParseCommand(uint64 serverConnectionHandlerID, QString cmd, QStringList args)
{
    if ((cmd.compare("CHANNEL_MUTER",Qt::CaseInsensitive)) != 0)
        return 1;

    uint64 channelID = (uint64)NULL;

    if (!args.isEmpty())
    {
        // TODO: first argument toggle/whitelist etc.
        // second argument server name
        // third argument channel name
    }

    toggleChannelMute(serverConnectionHandlerID, channelID);
    return 0;
}
