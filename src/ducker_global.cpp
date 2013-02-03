#include "ducker_global.h"
#include "ts3_functions.h"
#include "plugin.h"

#include <QSettings>

//Ducker_Global* Ducker_Global::m_Instance = 0;

Ducker_Global::Ducker_Global(QObject *parent) :
    m_isActive(false),
    m_value(0.0f)
{
    m_isPrintEnabled = true;
    this->setParent(parent);
    this->setObjectName("GlobalDucker");
    ts = TSFunctions::instance();
    talkers = Talkers::instance();
    vols = new Volumes(this);
    DuckTargets = new QMap<QString,QString>;
}

//Ducker_Global::~Ducker_Global()
//{
//}

void Ducker_Global::setActive(bool value)
{
    m_isActive = value;
    Log(QString("setActive: %1").arg((value)?"true":"false"));
    emit activeSet(m_isActive);
}

void Ducker_Global::AddMusicBot(uint64 serverConnectionHandlerID, anyID clientID)
{
    int talking;
    if(ts3Functions.getClientVariableAsInt(serverConnectionHandlerID, clientID, CLIENT_FLAG_TALKING, &talking) == ERROR_ok)
    {
        // Get My Id on this handler
        anyID myID;
        if(ts->GetClientId(serverConnectionHandlerID,&myID) != ERROR_ok)
            return;

        // Get My channel on this handler
        uint64 my_channelID;
        unsigned int error;
        if((error = ts3Functions.getChannelOfClient(serverConnectionHandlerID,myID,&my_channelID)) != ERROR_ok)
            ts->Error(serverConnectionHandlerID,error,"(Ducker_Global::AddMusicBot) Error getting Client Channel Id:");
        else
        {
            uint64 channelID;
            if((error = ts3Functions.getChannelOfClient(serverConnectionHandlerID,clientID,&channelID)) != ERROR_ok)
                ts->Error(serverConnectionHandlerID,error,"(Ducker_Global::AddMusicBot) Error getting Client Channel Id:");
            else
            {
                if (channelID!=my_channelID)
                    return;

                SimpleVolume* vol = AddMusicBotVolume(serverConnectionHandlerID,clientID);
                if (vol!=(SimpleVolume*)NULL)
                {
                    setActive(talking==STATUS_TALKING);
                    vol->setProcessing(talking==STATUS_TALKING);
                }
            }
        }
    }
}

//void Ducker_Global::AddMusicBot(QString uid)
//{
//    DuckTargets->insert(uid);
//}

void Ducker_Global::RemoveMusicBot(uint64 serverConnectionHandlerID, anyID clientID)
{
    QString uid;
    if (ts->GetClientUID(serverConnectionHandlerID,clientID,uid) != ERROR_ok)
        return;

    DuckTargets->remove(uid);
    vols->RemoveVolume(serverConnectionHandlerID,clientID);
}

//void Ducker_Global::RemoveMusicBot(QString uid)
//{
//    DuckTargets->remove(uid);
//}

void Ducker_Global::ToggleMusicBot(uint64 serverConnectionHandlerID, anyID clientID)
{
    QString uid;
    if (ts->GetClientUID(serverConnectionHandlerID,clientID,uid) != ERROR_ok)
        return;

    if (DuckTargets->contains(uid))
    {
        DuckTargets->remove(uid);
        vols->RemoveVolume(serverConnectionHandlerID,clientID);
    }
    else
    {
        char name[512];
        unsigned int error;
        if((error = ts3Functions.getClientDisplayName(serverConnectionHandlerID, clientID, name, 512)) != ERROR_ok)
        {
            ts->Error(serverConnectionHandlerID,error, "Error getting client display name");
            return;
        }
        DuckTargets->insert(uid,name);
        AddMusicBotVolume(serverConnectionHandlerID,clientID);
    }
    SaveDuckTargets();
}

bool Ducker_Global::isClientMusicBot(uint64 serverConnectionHandlerID, anyID clientID)
{
    QString uid;
    if (ts->GetClientUID(serverConnectionHandlerID,clientID,uid) != ERROR_ok)
        return false;

//    ts->Print(uid);
    return (DuckTargets->contains(uid));
}

void Ducker_Global::onClientMoveEvent(uint64 serverConnectionHandlerID, anyID clientID, uint64 oldChannelID, uint64 newChannelID, int visibility)
{
    // Get My Id on this handler
    anyID myID;
    if(ts->GetClientId(serverConnectionHandlerID,&myID) != ERROR_ok)
        return;

    if (clientID == myID)                   // I have switched channels
    {
        if (newChannelID == 0)
            return;

        vols->RemoveVolumes(serverConnectionHandlerID);

        // Get Channel Client List
        anyID* clients;
        ts->GetChannelClientList(serverConnectionHandlerID,newChannelID,&clients);

        // for every GDT client insert volume
        for(int i=0; clients[i]; i++)
        {
            if (clients[i] == myID)
                continue;

            if (isClientMusicBot(serverConnectionHandlerID,clients[i]))
                AddMusicBotVolume(serverConnectionHandlerID,clients[i]);
        }
    }
    else                                    // Someone else has...
    {
        // Get My channel on this handler
        uint64 channelID;
        unsigned int error;
        if((error = ts3Functions.getChannelOfClient(serverConnectionHandlerID,myID,&channelID)) != ERROR_ok)
            ts->Error(serverConnectionHandlerID,error,"(Ducker_Global::onClientMoveEvent) Error getting Client Channel Id:");
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
 * \brief CT_VolumeSuppression::onEditPlaybackVoiceDataEvent pre-processing voice event
 * \param serverConnectionHandlerID the connection id of the server
 * \param clientID the client-side runtime-id of the sender
 * \param samples the sample array to manipulate
 * \param sampleCount amount of samples in the array
 * \param channels currently always 1 on TS3; unused
 * \return true, if the ducker has processed
 */
bool Ducker_Global::onEditPlaybackVoiceDataEvent(uint64 serverConnectionHandlerID, anyID clientID, short *samples, int sampleCount, int channels)
{
    if (!(isRunning()))
        return false;

    if (!(vols->VolumesMap->contains(serverConnectionHandlerID)))
        return false;

    QMap<anyID,SimpleVolume*>* ConnectionHandlerVolumes = vols->VolumesMap->value(serverConnectionHandlerID);
    if (!ConnectionHandlerVolumes->contains(clientID))
        return false;

    SimpleVolume* vol = ConnectionHandlerVolumes->value(clientID);
    vol->process(samples,sampleCount);
    return true;
}

void Ducker_Global::onRunningStateChanged(bool value)
{
    if (value)
    {
        connect(talkers,SIGNAL(ConnectStatusChanged(uint64,int,uint)),vols,SLOT(onConnectStatusChanged(uint64,int,uint)));
        connect(talkers,SIGNAL(TalkStatusChanged(uint64,int,bool,anyID)),this,SLOT(onTalkStatusChanged(uint64,int,bool,anyID)));

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

                // Get My Id on this handler
                anyID myID;
                if(ts->GetClientId(*server,&myID) == ERROR_ok)
                {
                    // Get My channel on this handler
                    uint64 channelID;
                    unsigned int error;
                    if((error=ts3Functions.getChannelOfClient(*server,myID,&channelID)) != ERROR_ok)
                        ts->Error(*server,error,"Ducker_Global::onRunningStateChanged: Could not get channel of client.");
                    else
                        onClientMoveEvent(*server, myID, 0, channelID, RETAIN_VISIBILITY);
                }
            }
            ts3Functions.freeMemory(servers);

            bool is_active = false;
            QMapIterator<uint64,QMap<anyID,bool>* > i(*(talkers->GetTalkersMap()));
            while (i.hasNext())
            {
                i.next();
                if (i.value()->isEmpty())
                    continue;

                QMapIterator<anyID,bool> j(*i.value());
                while (j.hasNext())
                {
                    j.next();
                    if (!isClientMusicBot(i.key(),j.key()))
                    {
                        is_active = true;
                        break;
                    }
                }
                if (is_active)
                    break;
            }
            setActive(is_active);
        }
    }
    else
    {
        disconnect(talkers,SIGNAL(ConnectStatusChanged(uint64,int,uint)),vols,SLOT(onConnectStatusChanged(uint64,int,uint)));
        disconnect(talkers,SIGNAL(TalkStatusChanged(uint64,int,bool,anyID)),this,SLOT(onTalkStatusChanged(uint64,int,bool,anyID)));
        if (isActive())
            setActive(false);

        vols->RemoveVolumes();
    }
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

void Ducker_Global::onTalkStatusChanged(uint64 serverConnectionHandlerID, int status, bool isReceivedWhisper, anyID clientID)
{
    //    ts->Print(QString("(onTalkStatusChangeEvent): serverConnectionHandlerID: %1").arg(serverConnectionHandlerID));

    if (DuckTargets->isEmpty())
        return;

    anyID myID;
    if(ts->GetClientId(serverConnectionHandlerID,&myID) != ERROR_ok)
        return;

    if (clientID == myID)
        return;

    // Compute if this change causes a ducking change
    if ((!isActive()) && (status==STATUS_TALKING))
    {
        if (!isClientMusicBot(serverConnectionHandlerID,clientID))
            setActive(true);
    }
    else if (isActive() && (status==STATUS_NOT_TALKING))
    {
        // iterate if there are triggers left
        bool stayActive = false;

        QMapIterator<uint64,QMap<anyID,bool>* > i(*(talkers->GetTalkersMap()));
        while (i.hasNext())
        {
            i.next();
            if (i.value()->isEmpty())
                continue;

            QMapIterator<anyID,bool> j(*i.value());
            while (j.hasNext())
            {
                j.next();
                if (!isClientMusicBot(i.key(),j.key()))
                {
                    stayActive = true;
                    break;
                }
            }
            if (stayActive)
                break;
        }
        if (!(stayActive))
            setActive(false);
    }

    if (((status==STATUS_TALKING) || (status==STATUS_NOT_TALKING)) && (vols->VolumesMap->contains(serverConnectionHandlerID)))
    {
        QMap<anyID,SimpleVolume*>* ChanVolumes = vols->VolumesMap->value(serverConnectionHandlerID);
        if (ChanVolumes->contains(clientID))
        {
            SimpleVolume* vol = ChanVolumes->value(clientID);
            vol->setProcessing(status==STATUS_TALKING);
        }
    }
}

SimpleVolume* Ducker_Global::AddMusicBotVolume(uint64 serverConnectionHandlerID, anyID clientID)
{
    SimpleVolume* vol = vols->AddVolume(serverConnectionHandlerID,clientID);
    if (vol!=(SimpleVolume*)NULL)
    {
        vol->setGainDesiredByGainAdjuster(m_value);
        connect(this,SIGNAL(valueSet(float)),vol,SLOT(setGainDesiredByGainAdjuster(float)),Qt::DirectConnection);
        vol->setGainAdjustment(m_isActive);
        connect(this,SIGNAL(activeSet(bool)),vol,SLOT(setGainAdjustment(bool)),Qt::DirectConnection);
    }
    return vol;
}

void Ducker_Global::SaveDuckTargets()
{
    QSettings cfg(ts->GetFullConfigPath(), QSettings::IniFormat);
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
