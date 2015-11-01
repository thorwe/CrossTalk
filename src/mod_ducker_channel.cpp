#include "mod_ducker_channel.h"

#include "teamspeak/public_errors.h"
#include "teamspeak/public_errors_rare.h"
#include "teamspeak/public_definitions.h"
#include "teamspeak/public_rare_definitions.h"
#include "ts3_functions.h"

#include "plugin.h"
#include "ts_helpers_qt.h"

Ducker_Channel::Ducker_Channel(QObject *parent) :
    m_isActive(false),
    m_value(0.0f),
    m_homeId(0),
    m_isDuckPrioritySpeakers(false)
{
    m_isPrintEnabled = false;
    this->setParent(parent);
    this->setObjectName("ChannelDucker");
    talkers = Talkers::instance();
    vols = new Volumes(this,VolumeTypeDucker);
}

float Ducker_Channel::getValue() const
{
    return m_value;
}

bool Ducker_Channel::isTargetOtherTabs() const
{
    return m_isTargetOtherTabs;
}

bool Ducker_Channel::isDuckPrioritySpeakers() const
{
    return m_isDuckPrioritySpeakers;
}

// User Setting interaction

void Ducker_Channel::onRunningStateChanged(bool value)
{
    if (value)
    {
        connect(talkers,SIGNAL(ConnectStatusChanged(uint64,int,uint)),vols,SLOT(onConnectStatusChanged(uint64,int,uint)),Qt::UniqueConnection);

        uint64* servers;
        if(ts3Functions.getServerConnectionHandlerList(&servers) == ERROR_ok)
        {
            uint64* server;
            for(server = servers; *server != (uint64)NULL; ++server)
            {
                int status;
                if (ts3Functions.getConnectionStatus(*server, &status) != ERROR_ok)
                    continue;

                if (status != STATUS_CONNECTION_ESTABLISHED)
                    continue;

                unsigned int error;

                // Get My Id on this handler
                anyID myID;
                if((error = ts3Functions.getClientID(*server,&myID)) != ERROR_ok)
                {
                    Error("onRunningStateChanged",*server,error);
                    continue;
                }

                // Get My channel on this handler
                uint64 channelID;
                if((error=ts3Functions.getChannelOfClient(*server,myID,&channelID)) != ERROR_ok)
                    Error("(onRunningStateChanged) Could not get channel of client.",*server,error);
                else
                    onClientMoveEvent(*server, myID, 0, channelID, RETAIN_VISIBILITY, myID);

            }
            ts3Functions.freeMemory(servers);

            //UpdateActive();   //?
            talkers->DumpTalkStatusChanges(this,STATUS_TALKING); // Should work just fine?
        }
    }
    else
    {
        disconnect(talkers,SIGNAL(ConnectStatusChanged(uint64,int,uint)),vols,SLOT(onConnectStatusChanged(uint64,int,uint)));
        setActive(false);
        vols->RemoveVolumes();
    }
    Log(QString("enabled: %1").arg((value)?"true":"false"));
}

//! Set the amount of the volume reduction in dezibel(dB)
/*!
 * \brief Ducker_Channel::setValue
 * \param newValue reduction amount in dezibel (float)
 */
void Ducker_Channel::setValue(float newValue)
{
    if (newValue==m_value)
        return;

    m_value = newValue;
    Log(QString("setValue: %1").arg(m_value));
    emit valueSet(m_value);
}

void Ducker_Channel::setDuckingReverse(bool val)
{
    if (val==m_isTargetOtherTabs)
        return;

    m_isTargetOtherTabs = val;
    if (isRunning())  //since everything needs to be re-evaluated might as well toggle off/on
    {
        onRunningStateChanged(false); //setEnabled would trigger signal
        onRunningStateChanged(true);
    }
    Log(QString("isTargetOtherTabs: %1").arg((m_isTargetOtherTabs)?"true":"false"));
}

void Ducker_Channel::setDuckPrioritySpeakers(bool val)
{
    if (val==m_isDuckPrioritySpeakers)
        return;

    if (isRunning())  //since everything needs to be re-evaluated might as well toggle off/on
    {
        onRunningStateChanged(false); //setEnabled would trigger signal
        onRunningStateChanged(true);
    }
    m_isDuckPrioritySpeakers = val;
}

//void Ducker_Channel::saveSettings()
//{
//    QSettings cfg(TSHelpers::GetFullConfigPath(), QSettings::IniFormat);
//    cfg.setValue("ducking_enabled", isEnabled());
//    cfg.setValue("ducking_value", getValue());
//    cfg.setValue("ducking_reverse", isTargetOtherTabs());
//}

void Ducker_Channel::setHomeId(uint64 serverConnectionHandlerID)
{
    if (serverConnectionHandlerID == m_homeId)
        return;

    uint64 oldHomeId = m_homeId;
    m_homeId = serverConnectionHandlerID;
    if (m_homeId == 0)
        return;

    UpdateActive();
    // Dump talk changes for new and old home id
//    talkers->DumpTalkStatusChanges(this,STATUS_TALKING); //ToDo: ServerConnectionHandler specific dump
    // don't need no whisper or self talk here
    QMultiMap<uint64,anyID> map = talkers->GetTalkerMap();
    if (map.contains(oldHomeId))
    {
        QList<anyID> list = map.values(oldHomeId);
        for (int i = 0; i<list.size(); ++i)
        {
            DspVolumeDucker* vol = qobject_cast<DspVolumeDucker*>(vols->GetVolume(oldHomeId,list[i]));
            if (vol!=NULL)
                vol->setDuckBlocked(!m_isTargetOtherTabs);
        }
    }

    if (map.contains(m_homeId))
    {
        QList<anyID> list = map.values(m_homeId);
        for (int i = 0; i<list.size(); ++i)
        {
            DspVolumeDucker* vol = qobject_cast<DspVolumeDucker*>(vols->GetVolume(m_homeId,list[i]));
            if (vol!=NULL)
                vol->setDuckBlocked(m_isTargetOtherTabs);
        }
    }

    Log(QString("setHomeId: %1").arg(m_homeId));
}

void Ducker_Channel::setActive(bool value)
{
    if (value == m_isActive)
        return;

    m_isActive = value;
//    Log(QString("setActive: %1").arg((value)?"true":"false"),LogLevel_DEBUG);
    emit activeSet(m_isActive);
}

// ts event handlers

//! Have a volume object for everyone in the clients channel
void Ducker_Channel::onClientMoveEvent(uint64 serverConnectionHandlerID, anyID clientID, uint64 oldChannelID, uint64 newChannelID, int visibility, anyID myID)
{
    Q_UNUSED(visibility);

    unsigned int error;

    if (clientID == myID)                   // I have switched channels
    {
//        if (visibility == ENTER_VISIBILITY)  // those are direct from connect status change, filter those out to only use from setHomeId (and normal ones)
//            return;

//        if ((oldChannelID == 0)  && (visibility == RETAIN_VISIBILITY)) // -> setHomeId clientMove
//            return;

//        Log("Refreshing volumes",LogLevel_DEBUG);
        vols->RemoveVolumes(serverConnectionHandlerID);

        // Get Channel Client List
        anyID* clients;
        if((error = ts3Functions.getChannelClientList(serverConnectionHandlerID, newChannelID, &clients)) != ERROR_ok)
            Error("(onClientMoveEvent) Error getting Channel Client List", serverConnectionHandlerID, error);
        else
        {
            // for every client insert volume
            for(int i=0; clients[i]; i++)
            {
                if (clients[i] == myID)
                    continue;

                AddDuckerVolume(serverConnectionHandlerID,clients[i]);
            }
        }
    }
    else                                    // Someone else has...
    {
        // Get My channel on this handler
        uint64 channelID;
        if((error = ts3Functions.getChannelOfClient(serverConnectionHandlerID,myID,&channelID)) != ERROR_ok)
            Error(QString("(onClientMoveEvent) Error getting my Client Channel Id (clientID: %1)").arg(clientID), serverConnectionHandlerID, error);
        else
        {
            if (channelID == oldChannelID)      // left
                vols->RemoveVolume(serverConnectionHandlerID,clientID);
            else if (channelID == newChannelID) // joined
                AddDuckerVolume(serverConnectionHandlerID,clientID);
        }
    }
}

bool Ducker_Channel::onTalkStatusChanged(uint64 serverConnectionHandlerID, int status, bool isReceivedWhisper, anyID clientID, bool isMe)
{
    if (!isRunning())
        return false;

    if (isMe && !m_isTargetOtherTabs)
        return false;

    // Compute if this change causes a ducking change
    if ((!isActive()) && (status==STATUS_TALKING))
    {
        if ((isReceivedWhisper) || (!m_isTargetOtherTabs && (serverConnectionHandlerID != m_homeId)) || (m_isTargetOtherTabs && (serverConnectionHandlerID == m_homeId)))
            setActive(true);
    }
    else if (isActive() && (status==STATUS_NOT_TALKING))
        UpdateActive();

    if (isMe)
        return false;

    if (((status==STATUS_TALKING) || (status==STATUS_NOT_TALKING)))
    {
        DspVolumeDucker* vol = qobject_cast<DspVolumeDucker*>(vols->GetVolume(serverConnectionHandlerID,clientID));
        if (vol == NULL)
        {
//            Error("(onTalkStatusChanged) Could not get volume.",serverConnectionHandlerID,NULL);
            return false;
        }
        bool isTrigger = ((m_isTargetOtherTabs && (serverConnectionHandlerID == m_homeId)) || (!m_isTargetOtherTabs && (serverConnectionHandlerID != m_homeId)));

        //non ideal i guess
        unsigned int error;
        int isPrioritySpeakerDuck = 0;
        if ((!m_isDuckPrioritySpeakers) && (status==STATUS_TALKING) && ((error = ts3Functions.getClientVariableAsInt(serverConnectionHandlerID, clientID, CLIENT_IS_PRIORITY_SPEAKER, &isPrioritySpeakerDuck)) != ERROR_ok))
            Error("(onTalkStatusChangeEvent)",serverConnectionHandlerID,error);

        vol->setDuckBlocked(isTrigger || ((isReceivedWhisper || (isPrioritySpeakerDuck)) && (status==STATUS_TALKING)));
        vol->setProcessing(status==STATUS_TALKING);
        return ((status==STATUS_TALKING) && !(isReceivedWhisper || isPrioritySpeakerDuck));
    }
    return false;
}

//! Routes the arguments of the event to the corresponding volume object
/*!
 * \brief Ducker_Channel::onEditPlaybackVoiceDataEvent pre-processing voice event
 * \param serverConnectionHandlerID the connection id of the server
 * \param clientID the client-side runtime-id of the sender
 * \param samples the sample array to manipulate
 * \param sampleCount amount of samples in the array
 * \param channels amount of channels
 */
void Ducker_Channel::onEditPlaybackVoiceDataEvent(uint64 serverConnectionHandlerID, anyID clientID, short *samples, int sampleCount, int channels)
{
    if (!(isRunning()))
        return;

    if (((!m_isTargetOtherTabs) && (serverConnectionHandlerID != m_homeId)) || ((m_isTargetOtherTabs) && (serverConnectionHandlerID == m_homeId)))
        return;

    DspVolumeDucker* vol = qobject_cast<DspVolumeDucker*>(vols->GetVolume(serverConnectionHandlerID,clientID));
    if (vol == 0)
        return;

    //sampleCount = sampleCount * channels;
    vol->process(samples,sampleCount,channels);
}

//! Create and add a Volume object to the ServerChannelVolumes map
/*!
 * \brief Ducker_Channel::AddDuckerVolume Helper function
 * \param serverConnectionHandlerID the connection id of the server
 * \param clientID the client id
 * \return a volume object
 */
DspVolumeDucker* Ducker_Channel::AddDuckerVolume(uint64 serverConnectionHandlerID,anyID clientID)
{
    DspVolumeDucker* vol = qobject_cast<DspVolumeDucker*>(vols->AddVolume(serverConnectionHandlerID,clientID));
    if (vol != 0)
    {
        vol->setGainDesired(m_value);
        connect(this,SIGNAL(valueSet(float)),vol,SLOT(setGainDesiredByGainAdjuster(float)),Qt::DirectConnection);
        vol->setGainAdjustment(m_isActive);
        connect(this,SIGNAL(activeSet(bool)),vol,SLOT(setGainAdjustment(bool)),Qt::DirectConnection);
    }
//    Log(QString("Ducker: Added %1 to ServerChannelVolumes.").arg(clientID),serverConnectionHandlerID,LogLevel_DEBUG);
    return vol;
}

void Ducker_Channel::UpdateActive()
{
    bool is_active = false;

    if (m_isTargetOtherTabs && (talkers->isMeTalking() != 0))
        is_active = true;
    else if (!(talkers->GetWhisperMap().isEmpty()))
        is_active=true;
    else
    {
        if (m_isTargetOtherTabs)
        {
            if (talkers->GetTalkerMap().contains(m_homeId))
                is_active=true;
        }
        else
        {
            if (talkers->GetTalkerMap().count() != talkers->GetTalkerMap().count(m_homeId))
                is_active = true;
        }
    }
    setActive(is_active);
}
