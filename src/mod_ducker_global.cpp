#include "mod_ducker_global.h"

#include "teamspeak/public_errors.h"
#include "teamspeak/public_errors_rare.h"
#include "ts3_functions.h"

#include "plugin.h"

#include <QSettings>
#include "ts_helpers_qt.h"

Ducker_Global::Ducker_Global(QObject *parent) :
    m_isActive(false),
    m_value(0.0f),
    m_ContextMenuToggleMusicBot(-1)
{
    m_isPrintEnabled = false;
    this->setParent(parent);
    this->setObjectName("GlobalDucker");
    talkers = Talkers::instance();
    vols = new Volumes(this,VolumeTypeDucker);
    DuckTargets = new QMap<QString,QString>;
}

float Ducker_Global::getValue() const
{
    return m_value;
}

void Ducker_Global::setActive(bool value)
{
    if (value == m_isActive)
        return;

    m_isActive = value;
    //Log(QString("setActive: %1").arg((value)?"true":"false"),LogLevel_DEBUG);
    emit activeSet(m_isActive);
}

bool Ducker_Global::onInfoDataChanged(uint64 serverConnectionHandlerID, uint64 id, PluginItemType type, uint64 mine, QTextStream &data)
{
    if (!isRunning())
        return false;

    bool isDirty = false;
    if (type == PLUGIN_CLIENT)
    {
        ts3Functions.setPluginMenuEnabled(pluginID,m_ContextMenuToggleMusicBot,(id != mine)?1:0);

        if ((id != mine) && isClientMusicBot(serverConnectionHandlerID,(anyID)id))
        {
            data << this->objectName() << ":";
            isDirty = true;

            data << "Global Ducking Target (MusicBot)";
        }
    }
    return isDirty;
}

void Ducker_Global::onContextMenuEvent(uint64 serverConnectionHandlerID, PluginMenuType type, int menuItemID, uint64 selectedItemID)
{
    Q_UNUSED(type);
    if (menuItemID == m_ContextMenuToggleMusicBot)
        ToggleMusicBot(serverConnectionHandlerID,(anyID)selectedItemID);
}

void Ducker_Global::AddMusicBot(uint64 serverConnectionHandlerID, anyID clientID)
{
    int talking;
    if(ts3Functions.getClientVariableAsInt(serverConnectionHandlerID, clientID, CLIENT_FLAG_TALKING, &talking) == ERROR_ok)
    {
        unsigned int error;

        // Get My Id on this handler
        anyID myID;
        if((error = ts3Functions.getClientID(serverConnectionHandlerID,&myID)) != ERROR_ok)
        {
            Error("AddMusicBot",serverConnectionHandlerID,error);
            return;
        }

        // Get My channel on this handler
        uint64 my_channelID;
        if((error = ts3Functions.getChannelOfClient(serverConnectionHandlerID,myID,&my_channelID)) != ERROR_ok)
            Error("(AddMusicBot) Error getting Client Channel Id",serverConnectionHandlerID,error);
        else
        {
            uint64 channelID;
            if((error = ts3Functions.getChannelOfClient(serverConnectionHandlerID,clientID,&channelID)) != ERROR_ok)
                Error("(AddMusicBot) Error getting Client Channel Id",serverConnectionHandlerID,error);
            else
            {
                if (channelID!=my_channelID)
                    return;

                DspVolumeDucker* vol = qobject_cast<DspVolumeDucker*>(AddMusicBotVolume(serverConnectionHandlerID,clientID));
                if (vol != 0)
                {
                    if ((talking==STATUS_TALKING) && (!isActive()))
                        setActive(true);

                    vol->setProcessing(talking==STATUS_TALKING);
                }
            }
        }
    }
}

void Ducker_Global::RemoveMusicBot(uint64 serverConnectionHandlerID, anyID clientID)
{
    QString uid;
    if (TSHelpers::GetClientUID(serverConnectionHandlerID,clientID,uid) != ERROR_ok)
        return;

    DuckTargets->remove(uid);
    vols->RemoveVolume(serverConnectionHandlerID,clientID);
}

void Ducker_Global::ToggleMusicBot(uint64 serverConnectionHandlerID, anyID clientID)
{
    //Log(QString("(ToggleMusicBot) %1").arg(clientID),serverConnectionHandlerID,LogLevel_DEBUG);
    unsigned int error;

    QString uid;
    if (TSHelpers::GetClientUID(serverConnectionHandlerID,clientID,uid) != ERROR_ok)
    {
        Error("(ToggleMusicBot)");
        return;
    }

    if (DuckTargets->contains(uid))
    {
        DuckTargets->remove(uid);
        UpdateActive();
        vols->RemoveVolume(serverConnectionHandlerID,clientID);
    }
    else
    {
        char name[512];
        if((error = ts3Functions.getClientDisplayName(serverConnectionHandlerID, clientID, name, 512)) != ERROR_ok)
        {
            Error("(ToggleMusicBot) Error getting client display name",serverConnectionHandlerID,error);
            return;
        }
        DuckTargets->insert(uid,name);
        UpdateActive();
        AddMusicBotVolume(serverConnectionHandlerID,clientID);
    }
    SaveDuckTargets();
}

bool Ducker_Global::isClientMusicBot(uint64 serverConnectionHandlerID, anyID clientID)
{
    QString uid;
    if (TSHelpers::GetClientUID(serverConnectionHandlerID,clientID,uid) != ERROR_ok)
        return false;

    return (DuckTargets->contains(uid));
}

bool Ducker_Global::isClientMusicBotRt(uint64 serverConnectionHandlerID, anyID clientID)
{
    return vols->ContainsVolume(serverConnectionHandlerID,clientID);
}

void Ducker_Global::onClientMoveEvent(uint64 serverConnectionHandlerID, anyID clientID, uint64 oldChannelID, uint64 newChannelID, int visibility, anyID myID)
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
            // for every GDT client insert volume
            for(int i=0; clients[i]; i++)
            {
                if (clients[i] == myID)
                    continue;

                if (isClientMusicBot(serverConnectionHandlerID,clients[i]))
                    AddMusicBotVolume(serverConnectionHandlerID,clients[i]);
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
            {
                vols->RemoveVolume(serverConnectionHandlerID,clientID);
            }
            else if (channelID == newChannelID) // joined
            {
                if (isClientMusicBot(serverConnectionHandlerID,clientID))
                    AddMusicBotVolume(serverConnectionHandlerID,clientID);
            }
        }
    }
}

//! Routes the arguments of the event to the corresponding volume object
/*!
 * \brief Ducker_Global::onEditPlaybackVoiceDataEvent pre-processing voice event
 * \param serverConnectionHandlerID the connection id of the server
 * \param clientID the client-side runtime-id of the sender
 * \param samples the sample array to manipulate
 * \param sampleCount amount of samples in the array
 * \param channels currently always 1 on TS3; unused
 * \return true, if the ducker has processed / the client is a music bot
 */
bool Ducker_Global::onEditPlaybackVoiceDataEvent(uint64 serverConnectionHandlerID, anyID clientID, short *samples, int sampleCount, int channels)
{
    if (!(isRunning()))
        return false;

    DspVolumeDucker* vol = qobject_cast<DspVolumeDucker*>(vols->GetVolume(serverConnectionHandlerID,clientID));
    if (vol == 0)
        return false;

    vol->process(samples,sampleCount,channels);
    return (vol->isProcessing() && vol->getGainAdjustment());
}

void Ducker_Global::onRunningStateChanged(bool value)
{
    if(m_ContextMenuToggleMusicBot == -1)
    {
        m_ContextMenuToggleMusicBot = TSContextMenu::instance()->Register(this,PLUGIN_MENU_TYPE_CLIENT,"Toggle Global Ducking Target","duck_16.png");
        connect(TSContextMenu::instance(),SIGNAL(FireContextMenuEvent(uint64,PluginMenuType,int,uint64)),SLOT(onContextMenuEvent(uint64,PluginMenuType,int,uint64)),Qt::AutoConnection);
    }

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

            UpdateActive();
        }
    }
    else
    {
        disconnect(talkers,SIGNAL(ConnectStatusChanged(uint64,int,uint)),vols,SLOT(onConnectStatusChanged(uint64,int,uint)));
        setActive(false);
        vols->RemoveVolumes();
    }
    TSInfoData::instance()->Register(this,value,1);
    Log(QString("enabled: %1").arg((value)?"true":"false"));
}

void Ducker_Global::setValue(float newValue)
{
    if (newValue==m_value)
        return;

    m_value = newValue;
    Log(QString("setValue: %1").arg(m_value));
    emit valueSet(m_value);
}

void Ducker_Global::onTalkStatusChanged(uint64 serverConnectionHandlerID, int status, bool isReceivedWhisper, anyID clientID, bool isMe)
{
    Q_UNUSED(isReceivedWhisper);

    if (isMe || !isRunning())
        return;

    if (DuckTargets->isEmpty())
        return;

    // Compute if this change causes a ducking change
    if ((!isActive()) && (status==STATUS_TALKING))
    {
        if (!isClientMusicBotRt(serverConnectionHandlerID,clientID))
            setActive(true);
    }
    else if (isActive() && (status==STATUS_NOT_TALKING))
        UpdateActive();

    if ((status==STATUS_TALKING) || (status==STATUS_NOT_TALKING))
    {
        DspVolumeDucker* vol = qobject_cast<DspVolumeDucker*>(vols->GetVolume(serverConnectionHandlerID,clientID));
        if (vol != 0)
            vol->setProcessing(status==STATUS_TALKING);
    }
}

DspVolumeDucker* Ducker_Global::AddMusicBotVolume(uint64 serverConnectionHandlerID, anyID clientID)
{
    DspVolumeDucker* vol = qobject_cast<DspVolumeDucker*>(vols->AddVolume(serverConnectionHandlerID,clientID));
    if (vol != 0)
    {
        vol->setGainDesired(m_value);
        connect(this,SIGNAL(valueSet(float)),vol,SLOT(setGainDesiredByGainAdjuster(float)),Qt::DirectConnection);
        vol->setGainAdjustment(m_isActive);
        connect(this,SIGNAL(activeSet(bool)),vol,SLOT(setGainAdjustment(bool)),Qt::DirectConnection);
    }
    return vol;
}

void Ducker_Global::SaveDuckTargets()
{
    QSettings cfg(TSHelpers::GetFullConfigPath(), QSettings::IniFormat);
    cfg.beginGroup("ducker_global");
    int oldSize = cfg.beginReadArray("targets");
    cfg.endArray();
    cfg.beginWriteArray("targets", DuckTargets->size());
    QMapIterator<QString,QString> i(*(DuckTargets));
    int count = 0;
    while (i.hasNext())
    {
        i.next();
        cfg.setArrayIndex(count);
        cfg.setValue("uid",i.key());
        cfg.setValue("name",i.value());
        ++count;
    }
    if (oldSize > DuckTargets->size())
    {
        for (int j = DuckTargets->size(); j<oldSize; ++j)
        {
            cfg.setArrayIndex(j);
            cfg.remove("uid");
            cfg.remove("name");
        }
    }
    cfg.endArray();
    cfg.endGroup();
}

void Ducker_Global::UpdateActive()
{
    bool is_active = false;
//    QMapIterator<uint64,QMap<anyID,bool>* > i(*(talkers->GetTalkersMap()));
//    while (i.hasNext())
//    {
//        i.next();
//        if (i.value()->isEmpty())
//            continue;

//        QMapIterator<anyID,bool> j(*i.value());
//        while (j.hasNext())
//        {
//            j.next();
//            if (!isClientMusicBotRt(i.key(),j.key()))
//            {
//                is_active = true;
//                break;
//            }
//        }
//        if (is_active)
//            break;
//    }
    QMapIterator<uint64,anyID> i(talkers->GetTalkerMap());
    while (i.hasNext())
    {
        i.next();
        if (!isClientMusicBotRt(i.key(),i.value()))
        {
            is_active = true;
            break;
        }
    }
    if (!is_active)
    {
        i = talkers->GetWhisperMap();
        while (i.hasNext())
        {
            i.next();
            if (!isClientMusicBotRt(i.key(),i.value()))
            {
                is_active = true;
                break;
            }
        }
    }
    setActive(is_active);
}
