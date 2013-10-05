/*
 * Obviously, the core of this needs to have a certain amount of similarities to Mumble's Link plugin,
 * @ https://sourceforge.net/p/mumble/code/ci/master/tree/plugins/link/
 * which is under what appears to be a 3-clause BSD licence also operating under the alias of modified BSD license
 *
 * ToDo: Is this considered a modification, a derivation now?
 * Do I need to put his copyright above this, and/or does this need to be BSD too?
 * It's not like reading a struct is the invention of the wheel or sth.?!?
 */
#include "mod_positionalaudio.h"

#include "public_rare_definitions.h"
#include "public_errors.h"

#include "ts3_functions.h"
#include "../plugin.h"

#include "../talkers.h"
#include "../ts_helpers_qt.h"

#include "ts_serversinfo.h"

//bool operator==(const TS3_VECTOR &vec, const float *arr)
//{
//    return ((vec.x == arr[0]) && (vec.y == arr[1]) && (vec.z == arr[2]));
//}

struct LinkedMem {
    quint32 uiVersion;              //win:UINT32;posix:uint32_t
    ulong   dwcount;                //win:DWORD;posix:uint32_t  - ToDo: this is rather irritating, isn't it?
    float	fAvatarPosition[3];
    float	fAvatarFront[3];
    float	fAvatarTop[3];
    wchar_t	name[256];
    float	fCameraPosition[3];
    float	fCameraFront[3];
    float	fCameraTop[3];
    wchar_t	identity[256];
    quint32 context_len;            //win:UINT32;posix:uint32_t
    unsigned char context[256];
    wchar_t description[2048];
};

static LinkedMem *lm = NULL;

PositionalAudio::PositionalAudio(QObject *parent) :
    m_tryTimerId(0),
    m_fetchTimerId(0),
    m_fetchTimerTimeoutMsec(5000),
    m_sendCounter(0),
    m_silentSendCounter(2),
    m_Context_Dirty(false),
    m_isDirty_IdentityUncleaned(false),
    m_Avatar_Dirty(false),
    m_lastCount(0),
    m_isUseCamera(true),
    m_IsSendAllOverride(true)
{
    this->setParent(parent);
    this->setObjectName("PositionalAudio");
    m_isPrintEnabled = false;
    universe = new TsVrUniverse(this);
    meObj = new TsVrObjSelf(this);
    connect(meObj,SIGNAL(vrChanged(TsVrObj*,QString)),this,SLOT(onMyVrChanged(TsVrObj*,QString)));
    connect(meObj,SIGNAL(identityChanged(TsVrObj*,QString)),this,SLOT(onMyIdentityChanged(TsVrObj*,QString)));
    connect(meObj,SIGNAL(avatarChanged(TsVrObj*,TS3_VECTOR,TS3_VECTOR,TS3_VECTOR)),this,SLOT(onMyAvatarChanged(TsVrObj*,TS3_VECTOR,TS3_VECTOR,TS3_VECTOR)));
    NULL_VECTOR.x = 0.0f;
    NULL_VECTOR.y = 0.0f;
    NULL_VECTOR.z = 0.0f;
}

// Properties
QString PositionalAudio::getMyVr() const
{
    return meObj->getVr();
}

QString PositionalAudio::getMyIdentity() const
{
    return meObj->getIdentity();
}

bool PositionalAudio::isUseCamera() const
{
    return m_isUseCamera;
}

void PositionalAudio::setUseCamera(bool val)
{
    if (m_isUseCamera == val)
        return;
    m_isUseCamera = val;
    emit useCameraChanged(m_isUseCamera);
}

void PositionalAudio::AddServerSetting(QString serverUniqueId, QString serverName)
{
    if (m_ServerSettings.contains(serverUniqueId))
    {
        if (m_ServerSettings.value(serverUniqueId).serverName != serverName)
            m_ServerSettings[serverUniqueId].serverName = serverName;

        return;
    }

    PositionalAudio_ServerSettings setting;
    setting.serverName = serverName;
    setting.serverUniqueId = serverUniqueId;
    setting.enabled = false;
    setting.isBlocked = true;
    setting.sendInterval = 1.0f;
    m_ServerSettings.insert(serverUniqueId,setting);
}

void PositionalAudio::RemoveServerSetting(QString serverUniqueId, QString serverName)
{
    if (serverUniqueId == QString::null)
    {
        if (serverName == QString::null)
        {
            Error("(RemoveServerSetting) Called with no valid arguments");
            return;
        }
        QMapIterator<QString,PositionalAudio_ServerSettings> i(m_ServerSettings);
        unsigned int count = 0;
        while (i.hasNext())
        {
            i.next();
            PositionalAudio_ServerSettings setting = i.value();
            if (setting.serverName == serverName)
            {
                count++;
                serverUniqueId = setting.serverUniqueId;
            }
        }
        if (count > 1)
        {
            Error("(RemoveServerSetting) Multiple remove candidates found. Aborting.");
            return;
        }
        else if (count == 0)
        {
            Error("(RemoveServerSetting) No matching server setting found.");
            return;
        }
    }

    m_ServerSettings.remove(serverUniqueId);
    Log(QString("(RemoveServerSetting) %1").arg(serverName),LogLevel_INFO);
}

void PositionalAudio::setServerSettingEnabled(QString serverUniqueId, bool val)
{
    if (!m_ServerSettings.contains(serverUniqueId))
        return;
    if (m_ServerSettings.value(serverUniqueId).enabled == val)
        return;

    m_ServerSettings[serverUniqueId].enabled = val;
}

void PositionalAudio::setServerSettingBlocked(QString serverUniqueId, bool val)
{
    if (!m_ServerSettings.contains(serverUniqueId))
        return;
    if (m_ServerSettings.value(serverUniqueId).isBlocked == val)
        return;

    m_ServerSettings[serverUniqueId].isBlocked = val;
//    Print(QString("Block Status Changed: %1 %2").arg(m_ServerSettings.value(serverUniqueId).serverName).arg((m_ServerSettings.value(serverUniqueId).isBlocked)?"blocked":"unblocked"));
}

void PositionalAudio::setServerSettingSendInterval(QString serverUniqueId, float val)
{
    if (!m_ServerSettings.contains(serverUniqueId))
        return;
    if (m_ServerSettings.value(serverUniqueId).sendInterval == val)
        return;

    m_ServerSettings[serverUniqueId].sendInterval = val;
}

void PositionalAudio::setServerSettingSendIntervalSilentInc(QString serverUniqueId, float val)
{
    if (!m_ServerSettings.contains(serverUniqueId))
        return;
    if (m_ServerSettings.value(serverUniqueId).sendIntervalSilentInc == val)
        return;

    m_ServerSettings[serverUniqueId].sendIntervalSilentInc = val;
}

void PositionalAudio::onMyVrChanged(TsVrObj *obj, QString val)
{
    Q_UNUSED(obj);
    emit myVrChanged(val);
}

void PositionalAudio::onMyIdentityChanged(TsVrObj *obj, QString val)
{
    Q_UNUSED(obj);
    emit myIdentityChanged(val);
}

void PositionalAudio::onMyAvatarChanged(TsVrObj *obj, TS3_VECTOR position, TS3_VECTOR front, TS3_VECTOR top)
{
    Q_UNUSED(obj);
    Q_UNUSED(position);
    Q_UNUSED(front);
    Q_UNUSED(top);
    m_Avatar_Dirty = true;
}

QMap<QString, PositionalAudio_ServerSettings> PositionalAudio::getServerSettings() const
{
    return m_ServerSettings;
}

/*bool PositionalAudio::RegisterCustomEnvironmentSupport(QObject *p)
{
    CustomEnvironmentSupportInterface *iCustomEnvironmentSupport = qobject_cast<CustomEnvironmentSupportInterface *>(p);
    if (!iCustomEnvironmentSupport)
        return false;

    m_CustomEnvironmentSupportMap.insert(p->objectName(),p);
    return true;
}*/

// Other
void PositionalAudio::onClientMoveEvent(uint64 serverConnectionHandlerID, anyID clientID, uint64 oldChannelID, uint64 newChannelID, int visibility, anyID myID)
{
    Q_UNUSED(newChannelID);

    if (clientID==myID) // && newChannelID != 0 should be filtered out on plugin level already
    {
        QString args = GetSendString(true);
        if (!args.isEmpty())
            Send(serverConnectionHandlerID,args,PluginCommandTarget_CURRENT_CHANNEL,NULL,NULL);
    }
    else
    {
        bool isRemove = (visibility==LEAVE_VISIBILITY);
        if (!isRemove)
        {
            uint64 channelID;
            unsigned int error;
            if((error = ts3Functions.getChannelOfClient(serverConnectionHandlerID,myID,&channelID)) != ERROR_ok)
                Error("(onClientMoveEvent)",serverConnectionHandlerID,error);
            else
                isRemove = (oldChannelID==channelID);   // leave without losing visibility
        }
        if (isRemove)
        {
            universe->Remove(serverConnectionHandlerID,clientID);
            if (m_PlayersInMyContext.contains(serverConnectionHandlerID,clientID))
            {
                m_PlayersInMyContext.remove(serverConnectionHandlerID,clientID);
                ts3Functions.channelset3DAttributes(serverConnectionHandlerID, clientID, &NULL_VECTOR);
            }
        }
    }
}

void PositionalAudio::onCustom3dRolloffCalculationClientEvent(uint64 serverConnectionHandlerID, anyID clientID, float distance, float *volume)
{
    Q_UNUSED(serverConnectionHandlerID);
    Q_UNUSED(clientID);
    Q_UNUSED(distance);
    if ((isRunning()) && (!meObj->getVr().isEmpty()))
        *volume = 1.0f; // Who would want low volumes on a voicecom
}

bool PositionalAudio::onInfoDataChanged(uint64 serverConnectionHandlerID, uint64 id, PluginItemType type, uint64 mine, QTextStream &data)
{
    if (!isRunning())
        return false;

    bool isDirty = false;
    if (type == PLUGIN_CLIENT)
    {
        if (id == mine)
        {
            QString game = meObj->getVr();
            if (!game.isEmpty())
            {
                data << this->objectName() << ": ";
                isDirty = true;

                data << "is playing " << game;

                QString id = meObj->getIdentity();
                if (!id.isEmpty())
                    data << " as " << id;
            }
        }
        else
        {
            if (universe->Contains(serverConnectionHandlerID,(anyID)id))
            {
                TsVrObj* obj = universe->Get(serverConnectionHandlerID,(anyID)id);
                if (!obj->getVr().isEmpty())
                {
                    data << this->objectName() << ": ";
                    isDirty = true;

                    data << "is playing " << obj->getVr();
                    if (!obj->getIdentity().isEmpty())
                    {
                        data << " as " << obj->getIdentity();
                        if (m_PlayersInMyContext.contains(serverConnectionHandlerID,(anyID)id))
                        {
                            data << " in my context";
                        }
                        else
                            data << " outside of my context";
//                        data << ((m_PlayersInMyContext.contains(serverConnectionHandlerID,(anyID)id))?" in my context":" outside of my context");
                    }
                }
            }
        }
    }
    return isDirty;
}

/*int PositionalAudio::onServerErrorEvent(uint64 serverConnectionHandlerID, const char *errorMessage, unsigned int error, const char *returnCode, const char *extraMessage)
{
    Q_UNUSED(errorMessage);
    Q_UNUSED(returnCode);
    Q_UNUSED(extraMessage);

    if (isRunning() && (error==ERROR_client_is_flooding))    // check if im sending there once this is implemented
    {        
        QString sUId = TSServersInfo::instance()->GetServerInfo(serverConnectionHandlerID)->getUniqueId();
        emit serverBlock(sUId);
    }

    return 0;
}*/

void PositionalAudio::onRunningStateChanged(bool value)
{
    m_isDirty_IdentityUncleaned = false;    // not value, we want it to be off no matter what
    m_Context_Dirty = false;
    m_Avatar_Dirty = false;

    TSInfoData::instance()->Register(this,value,1);

    if (value)
    {
//        if (m_SendReturnCode.isEmpty())
//        {
//            m_SendReturnCodeC = new char[RETURNCODE_BUFSIZE];
//            ts3Functions.createReturnCode(pluginID,m_SendReturnCodeC,RETURNCODE_BUFSIZE);
//            m_SendReturnCode = QString::fromLatin1(m_SendReturnCodeC);
//            Print("Got returnCode: " + m_SendReturnCode);
//        }
        connect(Talkers::instance(),SIGNAL(ConnectStatusChanged(uint64,int,uint)),this,SLOT(onConnectStatusChanged(uint64,int,uint)),Qt::UniqueConnection);

//        Print(QString("DWORD: %1, quint32: %2, ulong: %3").arg(sizeof(DWORD)*8).arg(sizeof(quint32)*8).arg(sizeof(ulong)*8));
//        bool bCreated = false;

        m_sharedMemory = new QSharedMemory(this);
        m_sharedMemory->setNativeKey("MumbleLink");
        if (!m_sharedMemory->create(sizeof(LinkedMem),QSharedMemory::ReadWrite))
        {
            if (m_sharedMemory->error() == QSharedMemory::AlreadyExists)
            {
                if(!m_sharedMemory->attach(QSharedMemory::ReadWrite))
                {
                    Error(QString("Could not attach to shared memory: %1").arg(m_sharedMemory->errorString()));
                    return;
                }
            }
            else
            {
                Error(QString("Could not create shared memory: %1").arg(m_sharedMemory->errorString()));
                return;
            }
        }
        else
            memset(m_sharedMemory->data(), 0, m_sharedMemory->size());

        lm = static_cast<LinkedMem *>(m_sharedMemory->data());
        if (lm == NULL)
        {
            m_sharedMemory->detach();
            Error("Could not cast shared memory to struct.");
            return;
        }
        m_tryTimerId = this->startTimer(1000);
    }
    else
    {
        disconnect(Talkers::instance(),SIGNAL(ConnectStatusChanged(uint64,int,uint)),this,SLOT(onConnectStatusChanged(uint64,int,uint)));
        unlock();
        m_sharedMemory->detach();
        lm = NULL;
    }
    Log(QString("enabled: %1").arg((value)?"true":"false"));
}

void PositionalAudio::timerEvent(QTimerEvent *event)
{
    if (event->timerId() == m_fetchTimerId)
    {
        if (fetch())
        {
            if (m_fetchTimerElapsed != 0) // bool isFetchDirt =
                return;

            Update3DListenerAttributes();

            // send the data
            m_sendCounter++;
            Send();
            if (m_isDirty_IdentityUncleaned)
                TSInfoData::instance()->RequestSelfUpdate();
        }
        else
            unlock();
    }
    else if (event->timerId() == m_tryTimerId) // "trylock" in Mumble plugin API style
    {
        m_sharedMemory->lock();
        if ((lm->uiVersion == 1) || (lm->uiVersion == 2))
        {
            if (lm->dwcount != m_lastCount)
            {
                if (lm->description[0])
                {
                    QString vr_desc = QString::fromWCharArray(lm->description,2048);
                    int nullPos = vr_desc.indexOf(QChar::Null,0);
                    if (nullPos == -1)
                        Error("(UpdateMyGame) game description Null Terminator not found.");
                    else if (nullPos != 256)
                        vr_desc.truncate(nullPos);

                    meObj->setVrDescription(vr_desc);
                }
                else
                    meObj->setVrDescription(QString::null);

                if (lm->name[0])
                {
                    QString myVr = QString::fromWCharArray(lm->name,256);
                    int nullPos = myVr.indexOf(QChar::Null,0);
                    if (nullPos == -1)
                        Error("(UpdateMyGame) game Null Terminator not found.");
                    else if (nullPos != 256)
                        myVr.truncate(nullPos);

                    meObj->setVr(myVr);

                    if (!myVr.isEmpty())
                    {
                        // Print("Found " + meObj->getVr());
                        if (m_tryTimerId != 0)
                        {
                            this->killTimer(m_tryTimerId);
                            m_tryTimerId = 0;
                        }
                        m_lastCount = lm->dwcount;
                        m_fetchTimerElapsed = 0;
                        m_fetchTimerId = this->startTimer(FETCH_TIMER_INTERVAL);
                    }
                }
                else
                    meObj->setVr(QString::null);
            }
        }
        m_sharedMemory->unlock();
    }
}

void PositionalAudio::unlock()
{
    m_Context.clear();

    if (lm)
    {
        m_sharedMemory->lock();
        lm->dwcount = m_lastCount = 0;
        lm->uiVersion = 0;
        lm->name[0] = 0;
        lm->identity[0] = 0;
        lm->description[0] = 0;
        m_sharedMemory->unlock();
    }
    if (m_fetchTimerId != 0)
    {
        killTimer(m_fetchTimerId);
        m_fetchTimerId = 0;
    }
    if (isRunning() && m_tryTimerId == 0)
        m_tryTimerId = this->startTimer(1000);

    meObj->resetAvatar();
    meObj->resetCamera();

    m_PlayersInMyContext.clear();
    Update3DListenerAttributes();

    Send(QString::null,PluginCommandTarget_CURRENT_CHANNEL);
    meObj->setIdentityRaw(QString::null);
    meObj->setContext(QString::null);
    meObj->setVr(QString::null);
    meObj->setVrDescription(QString::null);
    Log("Unlocked.");
}

bool PositionalAudio::fetch()
{
    m_sharedMemory->lock();
    if ((lm->uiVersion != 1) && (lm->uiVersion != 2))
        return false;   // -> unlock

    if (lm->dwcount == m_lastCount)
    {
        m_fetchTimerElapsed = m_fetchTimerElapsed + FETCH_TIMER_INTERVAL;
//        Print(QString("fetch: elapsed: %1 timeout at: %2").arg(m_fetchTimerElapsed).arg(m_fetchTimerTimeoutMsec));
        m_sharedMemory->unlock();
        return !(m_fetchTimerElapsed > m_fetchTimerTimeoutMsec);    // -> unlock on false
    }
    m_fetchTimerElapsed = 0;
    m_lastCount = lm->dwcount;

//    m_Avatar_Dirty = (m_Avatar_Dirty || !((meObj->getAvatarPosition() == lm->fAvatarPosition) && (meObj->getAvatarFront() == lm->fAvatarFront) && (meObj->getAvatarTop() == lm->fAvatarTop)));

    meObj->setAvatar(lm->fAvatarPosition,lm->fAvatarFront,lm->fAvatarTop);

    if (lm->uiVersion == 2)
    {
        //*** Camera
        meObj->setCamera(lm->fCameraPosition,lm->fCameraFront,lm->fCameraTop);

        if (lm->context_len > 255)
            lm->context_len = 255;

        //*** Context
        QByteArray newContext = QByteArray(reinterpret_cast<const char *>(lm->context),lm->context_len);
        m_Context_Dirty = (m_Context != newContext);
        m_Context = newContext;
        if (m_Context_Dirty)
            meObj->setContext(m_Context.toHex().data());    //m_ContextHex = m_Context.toHex().data();

        //*** Identity
        QString newIdentity = QString::fromWCharArray(lm->identity,256);
        m_isDirty_IdentityUncleaned = (m_IdentityUncleaned != newIdentity);
        if (m_isDirty_IdentityUncleaned)
        {
            m_IdentityUncleaned = newIdentity;
            int nullPos = newIdentity.indexOf(QChar::Null,0);
            if (nullPos == -1)
                Error("identity Null Terminator not found.");
            else if (nullPos != 256)
                newIdentity.truncate(nullPos);

            meObj->setIdentityRaw(newIdentity);
        }
    }
    else if (lm->uiVersion == 1)    //legacy
    {
        meObj->setCamera(lm->fAvatarPosition,lm->fAvatarFront,lm->fAvatarTop);

        m_Context.clear();
        //m_ContextHex.clear();
        m_Context_Dirty = false;
        meObj->setContext(QString::null);
        m_IdentityUncleaned.clear();
        m_isDirty_IdentityUncleaned = false;
        meObj->setIdentityRaw(QString::null);
    }

    m_sharedMemory->unlock();
    return true;
}

bool PositionalAudio::onPluginCommand(uint64 serverConnectionHandlerID, anyID clientID, bool isMe, QString cmd, QTextStream &args)
{
    if (cmd != "3D")
        return false;

    if (isMe)
    {
        if (args.atEnd())
        {
            Print("I left vr.");
            return true;
        }

        TS3_VECTOR myAvatarPosition;
        TS3_VECTOR myAvatarFront;
        TS3_VECTOR myAvatarTop;
        args >> myAvatarPosition >> myAvatarFront >> myAvatarTop;

        if (!args.atEnd())
        {
            //name
            //context
            //identity
            QString args_stri = args.readAll().trimmed();
            QStringList list = args_stri.split("[Ct_Delimiter]",QString::KeepEmptyParts,Qt::CaseSensitive);    // keep empty parts?
            QString name = list.at(0);
            if (list.size() > 1)
            {
                QString rest = list.at(1);
                QTextStream in(&rest);

                QString context;
                in >> context;

                QString identity = in.readAll().trimmed();

                Print(QString("Received (me): cId: %1 VR: %2 CO: %3 ID: %4").arg(clientID).arg(name).arg((context == meObj->getContext())?"match":"no match -.-").arg(identity), serverConnectionHandlerID, LogLevel_DEBUG);
            }
            else    //version 1
            {
                Print(QString("Received (me): cId: %1 VR: %2").arg(clientID).arg(name), serverConnectionHandlerID, LogLevel_DEBUG);
            }
        }
//        else
//            Print("Received(me) AV",serverConnectionHandlerID,LogLevel_DEBUG);

        return true;
    }

    TsVrObjOther* obj = universe->Get(serverConnectionHandlerID,clientID);
    if (!obj)
    {
        obj = universe->Add(serverConnectionHandlerID,clientID);
        m_IsSendAllOverride = true;
    }

    if (args.atEnd())   // Player left vr (unlocked)
    {
        Log(QString("%1 left %2 VR.").arg(obj->getIdentity()).arg(obj->getVr()));
        universe->Remove(serverConnectionHandlerID,clientID);
        m_PlayersInMyContext.remove(serverConnectionHandlerID,clientID);
        return true;
    }

    //args >> obj->avatarPosition >> obj->avatarFront >> obj->avatarTop;
    TS3_VECTOR avatarPosition;
    TS3_VECTOR avatarFront;
    TS3_VECTOR avatarTop;
    args >> avatarPosition >> avatarFront >> avatarTop;
    obj->setAvatar(avatarPosition, avatarFront, avatarTop);

    if (!args.atEnd())
    {
        //name
        //context
        //identity
        QString args_stri = args.readAll().trimmed();
        QStringList list = args_stri.split("[Ct_Delimiter]",QString::KeepEmptyParts,Qt::CaseSensitive);    // keep empty parts?
        QString name = list.at(0);

        bool isDirtyName = (name != obj->getVr());
        obj->setVr(name);
        bool isDirtyContext = false;
        bool isDirtyId = false;

        if (list.size() > 1)
        {
            QString rest = list.at(1);
            QTextStream in(&rest);

            QString context;
            in >> context;
            isDirtyContext = (context != obj->getContext());
            obj->setContext(context);

            QString identity = in.readAll().trimmed();
            isDirtyId = (identity != obj->getIdentity());
            obj->setIdentityRaw(identity);

            Log(QString("Received: VR: %2 CO: %3 ID: %4").arg(name).arg((context == meObj->getContext())?"match":"no match").arg(identity), serverConnectionHandlerID, LogLevel_DEBUG);

            if (isDirtyName || isDirtyContext)
            {
                if (meObj->getVr() == obj->getVr())
                {
                    if (obj->getContext() == meObj->getContext())
                    {
                        if (!m_PlayersInMyContext.contains(serverConnectionHandlerID,clientID))
                            m_PlayersInMyContext.insert(serverConnectionHandlerID,clientID);
                    }
                    else if (m_PlayersInMyContext.contains(serverConnectionHandlerID,clientID))
                        m_PlayersInMyContext.remove(serverConnectionHandlerID,clientID);
                }
                else
                    m_PlayersInMyContext.remove(serverConnectionHandlerID,clientID);
            }
        }
        else    //version 1
        {
            Print(QString("Received: cId: %1 VR: %2").arg(clientID).arg(name), serverConnectionHandlerID, LogLevel_DEBUG);
        }

        if (isDirtyName || isDirtyContext || isDirtyId)
            TSInfoData::instance()->RequestUpdate(serverConnectionHandlerID,clientID);
    }

    if (m_PlayersInMyContext.contains(serverConnectionHandlerID,clientID))
        ts3Functions.channelset3DAttributes(serverConnectionHandlerID,clientID,&obj->getAvatarPosition());

    return true;
}

QString PositionalAudio::GetSendString(bool isAll)
{
    // Bulk version
    if (meObj->getVr().isEmpty())
        return QString::null;

    QString out_stri;
    QTextStream out(&out_stri);
    out << meObj->getAvatarPosition() << meObj->getAvatarFront() << meObj->getAvatarTop();
    if (isAll)
    {
//        out << " " << m_GameName;
        out << " " << meObj->getVr();

        if (lm->uiVersion == 2)
        {
//            out << "[Ct_Delimiter]" << (m_ContextHex.isEmpty()?"[Ct_None]":m_ContextHex);
            QString myContext = meObj->getContext();
            out << "[Ct_Delimiter]" << (myContext.isEmpty()?"[Ct_None]":myContext);
            QString my_ident = meObj->getIdentityRaw();
            if (!my_ident.isEmpty())
                out << " " << my_ident;
        }
    }
    return out_stri;
}

//! Non-throttled DoSend
void PositionalAudio::Send(uint64 serverConnectionHandlerID, QString args, int targetMode, const anyID *targetIDs, const char *returnCode)
{
    unsigned int error;

    // needed to be check beforehand
//    int status;
//    if ((error = ts3Functions.getConnectionStatus(serverConnectionHandlerID,&status)) != ERROR_ok)
//    {
//        Error("(Send)",serverConnectionHandlerID,error);
//        return;
//    }
//    if (status != STATUS_CONNECTION_ESTABLISHED)
//        return;

    anyID myID;
    if ((error = ts3Functions.getClientID(serverConnectionHandlerID,&myID)) != ERROR_ok)
    {
        Error("(Send)",serverConnectionHandlerID,error);
        return;
    }

    QString cmd;
    QTextStream str(&cmd);
    if (args.isEmpty())
        str << myID << " 3D";   // left game (unlocked)
    else
        str << myID << " 3D " << args;

//    Print(QString("Sending: %1").arg(cmd),serverConnectionHandlerID,LogLevel_DEBUG);
//    returnCode = m_SendReturnCodeC;
    ts3Functions.sendPluginCommand(serverConnectionHandlerID,pluginID,cmd.toAscii().constData(),targetMode,targetIDs,returnCode);
}

void PositionalAudio::Send(QString args, int targetMode)
{
    if (!m_ServerSettings.contains("default"))
        return;

    // Get clients in my context
    uint64* servers;
    if(ts3Functions.getServerConnectionHandlerList(&servers) == ERROR_ok)
    {
//        m_silentSendCounter++;

        uint64 myTalkingScHandler = Talkers::instance()->isMeTalking();

        uint64* server;
        for(server = servers; *server != (uint64)NULL; ++server)
        {
            TSServerInfo* serverInfo = TSServersInfo::instance()->GetServerInfo(*server);
            if (serverInfo == (TSServerInfo*)NULL)
                continue;

//            unsigned int error;
//            int status;
//            if ((error = ts3Functions.getConnectionStatus(*server,&status)) != ERROR_ok)
//            {
//                Error("(Send)",*server,error);
//                continue;
//            }
//            if (status != STATUS_CONNECTION_ESTABLISHED)
//            {
//                Error(QString("(Send) %1 not connected").arg(*server));
//                continue;
//            }

            QString sUId = serverInfo->getUniqueId();
            const PositionalAudio_ServerSettings s_settings = (m_ServerSettings.contains(sUId)?m_ServerSettings.value(sUId):m_ServerSettings.value("default"));
            if (!s_settings.enabled)
                continue;

            if (targetMode == PluginCommandTarget_CLIENT)
            {
                if ((m_ServerSettings.contains(sUId)) && m_ServerSettings.value(sUId).isBlocked)    //don't fallback on "default", which should never be isBlocked
                    continue;

                int count_max = (s_settings.sendInterval) * SEND_INTERVAL_MODIFIER; // with SEND_THROTTLE_GLOBAL == 5 normalized; todo dynamic
                if (myTalkingScHandler != *server)
                {
//                    Print("Adding silent interval increase");
                    count_max += (s_settings.sendIntervalSilentInc * SEND_INTERVAL_MODIFIER);
                }

                if (m_SendCounters[*server]++ < count_max)
                    continue;

//                Print(QString("count: %1; max count: %2").arg(m_SendCounters[*server]+1).arg(count_max));
                m_SendCounters[*server] = 0;

//                QList<anyID> list = m_PlayersInMyContext.values(*server);
//                if (!list.isEmpty())
//                {
//                    int size = list.size();
//                    anyID* targetIds = new anyID[size+1];
//                    for (int i=0; i<size; ++i)
//                        targetIds[i] = list.at(i);

//                    anyID myID;
//                    unsigned int error;
//                    if ((error = ts3Functions.getClientID(*server,&myID)) != ERROR_ok)
//                    {
//                        Error("(SendAV)",*server,error);
//                        return;
//                    }
//                    Print(QString("(SendAV): clients: %1").arg(list.size()),*server,LogLevel_DEBUG);
//                    targetIds[size]=myID;   //testing; when removing also change size+1 to size above
//                    Send(*server, args, targetMode, targetIds, NULL);
//                    delete [] targetIds;
//                }
                Send(*server, args, PluginCommandTarget_CURRENT_CHANNEL, NULL, NULL);
            }
            else
                Send(*server, args, targetMode, NULL, NULL);
        }
        ts3Functions.freeMemory(servers);
    }
    m_sendCounter = 0;
}

void PositionalAudio::Send()
{
    if (m_Context_Dirty || m_isDirty_IdentityUncleaned || m_IsSendAllOverride)
    {
        if (m_Context_Dirty)
            Log(QString("New context: %1").arg(meObj->getContext()));

        if (m_isDirty_IdentityUncleaned)
            Log("New identity: " + meObj->getIdentityRaw());

        m_IsSendAllOverride = false;
        QString args = GetSendString(true);
        if (!args.isEmpty())
        {
            m_Avatar_Dirty = false;
            m_Context_Dirty = false;
            m_isDirty_IdentityUncleaned = false;
            Send(args,PluginCommandTarget_CURRENT_CHANNEL);
        }
    }
    else if (m_sendCounter > SEND_THROTTLE_GLOBAL)    //was: 50
    {
        m_silentSendCounter++;
        if (m_Avatar_Dirty)
        {
            QString args = GetSendString(false);
            if (!args.isEmpty())
            {
                m_Avatar_Dirty = false;
                Send(args,PluginCommandTarget_CLIENT);
            }
        }
    }
}

void PositionalAudio::Update3DListenerAttributes()
{
    uint64* servers;
    if(ts3Functions.getServerConnectionHandlerList(&servers) == ERROR_ok)
    {
        QString myVr = meObj->getVr();
        QString myContext = meObj->getContext();
        uint64* server;
        for(server = servers; *server != (uint64)NULL; ++server)
        {
            int status;
            if (ts3Functions.getConnectionStatus(*server, &status) != ERROR_ok)
                continue;

            if (status != STATUS_CONNECTION_ESTABLISHED)
                continue;

            if (m_isUseCamera)
                ts3Functions.systemset3DListenerAttributes(*server,&meObj->getCameraPosition(),&meObj->getCameraFront(),&meObj->getCameraTop());
            else
                ts3Functions.systemset3DListenerAttributes(*server,&meObj->getAvatarPosition(),&meObj->getAvatarFront(),&meObj->getAvatarTop());

            unsigned int error;
            // Get My Id on this handler
            anyID myID;
            if((error = ts3Functions.getClientID(*server,&myID)) != ERROR_ok)
                Error("(Update3DListenerAttributes)");
            else
            {
                // Get My channel on this handler
                uint64 channelID;
                if((error=ts3Functions.getChannelOfClient(*server,myID,&channelID)) != ERROR_ok)
                    Error("(Update3DListenerAttributes)",*server,error);
                else
                {
                    // Get Channel Client List
                    anyID* clients;
                    if((error = ts3Functions.getChannelClientList(*server, channelID, &clients)) != ERROR_ok)
                        Error("(Update3DListenerAttributes)", *server, error);
                    else
                    {
                        if (m_Context_Dirty)
                            m_PlayersInMyContext.clear();

                        for(int i=0; clients[i]; i++)
                        {
                            if (m_Context_Dirty)    // Refill m_PlayersInMyContext
                            {
                                if (universe->Contains(*server,clients[i]))
                                {
                                    TsVrObj* obj = universe->Get(*server,clients[i]);
                                    if ((myVr == obj->getVr()) && (myContext == obj->getContext()))
                                        m_PlayersInMyContext.insert(*server,clients[i]);
                                }
                            }

                            if (!m_PlayersInMyContext.contains(*server,clients[i]))
                            {
                                if (m_isUseCamera)
                                    ts3Functions.channelset3DAttributes(*server,clients[i],&meObj->getCameraPosition());
                                else
                                    ts3Functions.channelset3DAttributes(*server,clients[i],&meObj->getAvatarPosition());
                            }
                        }
                    }
                }
            }
        }
        ts3Functions.freeMemory(servers);
    }
}

void PositionalAudio::onConnectStatusChanged(uint64 serverConnectionHandlerID, int newStatus, unsigned int errorNumber)
{
    Q_UNUSED(errorNumber);

    if (newStatus == STATUS_CONNECTION_ESTABLISHED)
    {
        m_SendCounters.insert(serverConnectionHandlerID,0);
        unsigned int error;
        // Set system 3d settings
        if((error = ts3Functions.systemset3DSettings(serverConnectionHandlerID, 1.0f, 0.1f)) != ERROR_ok)
            Error("(onConnectStatusChanged)",serverConnectionHandlerID,error);
        else
            Log("System 3D Settings set.");
    }
    else if (newStatus == STATUS_DISCONNECTED)
    {
        universe->Remove(serverConnectionHandlerID);
        m_PlayersInMyContext.remove(serverConnectionHandlerID);
        m_SendCounters.remove(serverConnectionHandlerID);
    }
}

QTextStream &operator<<(QTextStream &out, const TS3_VECTOR &ts3Vector)
{
    out << " " << ts3Vector.x << " " << ts3Vector.y << " " << ts3Vector.z;
    return out;
}

QTextStream &operator>>(QTextStream &in, TS3_VECTOR &ts3Vector)
{
    in >> ts3Vector.x >> ts3Vector.y >> ts3Vector.z;
    return in;
}
