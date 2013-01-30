#include "ct_volumesuppression.h"

#include "public_errors.h"
#include "public_errors_rare.h"
#include "public_definitions.h"
#include "public_rare_definitions.h"
#include "ts3_functions.h"

#include "plugin.h"
#include "tsfunctions.h"

CT_VolumeSuppression::CT_VolumeSuppression(QObject *parent) :
//    QObject(parent),
    m_isActive(false),
    m_value(0.0f),
    m_homeId(0)
{
    m_isPrintEnabled = true;
    this->setParent(parent);
    this->setObjectName("ChannelDucker");
    ts = TSFunctions::instance();
    talkers = Talkers::instance();
    vols = new Volumes(this);
}

// User Setting interaction

void CT_VolumeSuppression::onRunningStateChanged(bool value)
{
    connect(talkers,SIGNAL(TalkStatusChanged(uint64,int,bool,anyID)),this,SLOT(onTalkStatusChanged(uint64,int,bool,anyID)));
    if (value)
    {
        connect(talkers,SIGNAL(ConnectStatusChanged(uint64,int,uint)),vols,SLOT(onConnectStatusChanged(uint64,int,uint)));
//        connect(talkers,SIGNAL(TalkStatusChanged(uint64,int,bool,anyID)),this,SLOT(onTalkStatusChanged(uint64,int,bool,anyID)));
//        // TODO: Get Talking clients and iterate
    }
    else
    {
        disconnect(talkers,SIGNAL(ConnectStatusChanged(uint64,int,uint)),vols,SLOT(onConnectStatusChanged(uint64,int,uint)));
//        disconnect(talkers,SIGNAL(TalkStatusChanged(uint64,int,bool,anyID)),this,SLOT(onTalkStatusChanged(uint64,int,bool,anyID)));
        if (isActive())
            setActive(false);

        vols->RemoveVolumes();
    }
    Log(QString("enabled: %1").arg((value)?"true":"false"));
}

//! Set the amount of the volume reduction in dezibel(dB)
/*!
 * \brief CT_VolumeSuppression::setValue
 * \param newValue reduction amount in dezibel (float)
 */
void CT_VolumeSuppression::setValue(float newValue)
{
    if (newValue==m_value)
        return;

    m_value = newValue;
    Log(QString("setValue: %1").arg(m_value));
    emit valueSet(m_value);
}

void CT_VolumeSuppression::setDuckingReverse(bool val)
{
    m_isReverse = val;
    Log(QString("isReverse: %1").arg((m_isReverse)?"true":"false"));
}

void CT_VolumeSuppression::setHomeId(uint64 serverConnectionHandlerID)
{
    if (serverConnectionHandlerID == m_homeId)
        return;
    m_homeId = serverConnectionHandlerID;

    // Get My Id on this handler
    anyID myID;
    if(ts->GetClientId(serverConnectionHandlerID,&myID) == ERROR_ok)
    {
        // Get My channel on this handler
        uint64 channelID;
        unsigned int error;
        if((error = ts3Functions.getChannelOfClient(serverConnectionHandlerID,myID,&channelID)) != ERROR_ok)
        {
            ts->Error(serverConnectionHandlerID,error,"Error getting Client Channel Id:");
        }
        else
        {
            onClientMoveEvent(serverConnectionHandlerID,myID,0,channelID,RETAIN_VISIBILITY);
        }
    }

    Log(QString("setHomeId: %1").arg(m_homeId));
}

void CT_VolumeSuppression::setActive(bool value)
{
    m_isActive = value;
    Log(QString("setActive: %1").arg((value)?"true":"false"));
    emit activeSet(m_isActive);
}

// ts event handlers

void CT_VolumeSuppression::onClientMoveEvent(uint64 serverConnectionHandlerID, anyID clientID, uint64 oldChannelID, uint64 newChannelID, int visibility)
{
    if (ducker_G->isEnabled() && ducker_G->isClientMusicBot(serverConnectionHandlerID,clientID))
        return;

    // Get My Id on this handler
    anyID myID;
    if(ts->GetClientId(serverConnectionHandlerID,&myID) != ERROR_ok)
        return;

    if (clientID == myID)                   // I have switched channels
    {
        if (newChannelID == 0)
            return;

        if (((!m_isReverse) && (serverConnectionHandlerID != m_homeId)) || ((m_isReverse) && (serverConnectionHandlerID == m_homeId)))
            return;

        vols->RemoveVolumes(serverConnectionHandlerID);

        // Get Channel Client List
        anyID* clients;
        ts->GetChannelClientList(serverConnectionHandlerID,newChannelID,&clients);

        // for every client insert volume
        for(int i=0; clients[i]; i++)
        {
            if (clients[i] == myID)
                continue;

            AddDuckerVolume(serverConnectionHandlerID,clients[i]);
        }
    }
    else                                    // Someone else has...
    {
        // Get My channel on this handler
        uint64 channelID;
        unsigned int error;
        if((error = ts3Functions.getChannelOfClient(serverConnectionHandlerID,myID,&channelID)) != ERROR_ok)
        {
            ts->Error(serverConnectionHandlerID,error,"(CT_VolumeSuppression::onClientMoveEvent) Error getting Client Channel Id:");
        }
        else
        {
            if (channelID == oldChannelID)      // left
                vols->RemoveVolume(serverConnectionHandlerID,clientID);
            else if (channelID == newChannelID) // joined
            {
                AddDuckerVolume(serverConnectionHandlerID,clientID);
            }
        }
    }
}

void CT_VolumeSuppression::onTalkStatusChanged(uint64 serverConnectionHandlerID, int status, bool isReceivedWhisper, anyID clientID)
{
//    ts->Print(QString("(onTalkStatusChangeEvent): serverConnectionHandlerID: %1").arg(serverConnectionHandlerID));
    if (ducker_G->isEnabled() && ducker_G->isClientMusicBot(serverConnectionHandlerID,clientID))
        return;

    anyID myID;
    if(ts->GetClientId(serverConnectionHandlerID,&myID) != ERROR_ok)
        return;

    if (clientID == myID)
        return;

    // Compute if this change causes a ducking change
    if ((!isActive()) && (status==STATUS_TALKING))
    {
        if ((isReceivedWhisper) || (!m_isReverse && (serverConnectionHandlerID != m_homeId)) || (m_isReverse && (serverConnectionHandlerID == m_homeId)))
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

            if (((!m_isReverse) && (i.key() != m_homeId)) || ((m_isReverse) && (i.key() == m_homeId)))
            {
//                    ts->Print("Staying active because there are talkers.");
                    stayActive = true;
                    break;
            }
            else
            {
                //if ((!(i.value()->isEmpty())) && (-1 != (i.value()->key(true,-1)))) //-1 exceeds anyID; unsure if 0 would work
                QList<anyID> whisp = i.value()->keys(true);
                if (whisp.count() > 0)
                {
                    ts->Print("Staying active because a whisperer has been found.");
                    ts->Print(QString("Amount whisperers: %1").arg(whisp.count()));
                    stayActive = true;
                    break;
                }
            }
        }

        // Let's try this instead: (should be working, but somewhat inefficient in comparison)
/*        int talking;
        uint64* serverList;
        if(ts3Functions.getServerConnectionHandlerList(&serverList) == ERROR_ok)
        {
            for(int i=0; serverList[i] != NULL; ++i)
            {
                int status;
                if (ts3Functions.getConnectionStatus(serverList[i], &status) != ERROR_ok)
                    continue;

                if (status != STATUS_CONNECTION_ESTABLISHED)
                    continue;

                //Get all visible clients
                anyID *clientList;
                if (ts3Functions.getClientList(serverList[i], &clientList) != ERROR_ok)
                    continue;

                for (int j=0; clientList[j]!=NULL ; ++j)
                {
                    if(ts3Functions.getClientVariableAsInt(serverList[i], clientList[j], CLIENT_FLAG_TALKING, &talking) != ERROR_ok)
                        continue;

                    if (talking != STATUS_TALKING)
                        continue;

                    if (serverList[i] != m_homeId)
                    {
                        stayActive = true;
                        break;
                    }
                    else
                    {
                        int isWhispering;
                        if ((ts3Functions.isWhispering(serverList[i], clientList[j], &isWhispering)) == ERROR_ok)
                        {
                            if (isWhispering == 1)
                            {
                                stayActive = true;
                                break;
                            }
                        }
                    }
                }
                ts3Functions.freeMemory(clientList);
            }
            ts3Functions.freeMemory(serverList);
        }*/
        if (!(stayActive))
            setActive(false);
    }

    if (((status==STATUS_TALKING) || (status==STATUS_NOT_TALKING)) && (vols->VolumesMap->contains(serverConnectionHandlerID)))
    {
        QMap<anyID,SimpleVolume*>* ChanVolumes = vols->VolumesMap->value(serverConnectionHandlerID);
        if (ChanVolumes->contains(clientID))
        {
            SimpleVolume* vol = ChanVolumes->value(clientID);
            vol->setDuckBlocked((isReceivedWhisper) && (status==STATUS_TALKING));
            vol->setProcessing(status==STATUS_TALKING);
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
 */
void CT_VolumeSuppression::onEditPlaybackVoiceDataEvent(uint64 serverConnectionHandlerID, anyID clientID, short *samples, int sampleCount, int channels)
{
    if (!(isRunning()))
        return;

    if (((!m_isReverse) && (serverConnectionHandlerID != m_homeId)) || ((m_isReverse) && (serverConnectionHandlerID == m_homeId)))
        return;


    if (!vols->VolumesMap->contains(serverConnectionHandlerID))
        return;

    QMap<anyID,SimpleVolume*>* ChanVolumes = vols->VolumesMap->value(serverConnectionHandlerID);
    if (!ChanVolumes->contains(clientID))
        return;

    SimpleVolume* vol = ChanVolumes->value(clientID);
    vol->process(samples,sampleCount);
}

//! Create and add a Volume object to the ServerChannelVolumes map
/*!
 * \brief CT_VolumeSuppression::AddDuckerVolume Helper function
 * \param serverConnectionHandlerID the connection id of the server
 * \param clientID the client id
 * \return a volume object
 */
SimpleVolume* CT_VolumeSuppression::AddDuckerVolume(uint64 serverConnectionHandlerID,anyID clientID)
{
    SimpleVolume* vol = vols->AddVolume(serverConnectionHandlerID,clientID);
    if (vol!=(SimpleVolume*)NULL)
    {
        vol->setGainDesiredByGainAdjuster(m_value);
        connect(this,SIGNAL(valueSet(float)),vol,SLOT(setGainDesiredByGainAdjuster(float)),Qt::DirectConnection);
        vol->setGainAdjustment(m_isActive);
        connect(this,SIGNAL(activeSet(bool)),vol,SLOT(setGainAdjustment(bool)),Qt::DirectConnection);
    }
    //    ts->Print(QString("Ducker: Added %1 to ServerChannelVolumes.").arg(clientID));
    return vol;
}

