/*
 * Obviously, the core of this needs to have a certain amount of similarities to Mumble's Link plugin,
 * @ https://sourceforge.net/p/mumble/code/ci/master/tree/plugins/link/
 * which is under what appears to be a 3-clause BSD licence also operating under the alias of modified BSD license
 *
 */
#include "mod_positionalaudio.h"

#include "teamspeak/public_rare_definitions.h"
#include "teamspeak/public_errors.h"

#include "ts3_functions.h"
#include "plugin.h"

#include "core/ts_helpers_qt.h"

#include "core/ts_serversinfo.h"

#include "volume/db.h"

#ifndef M_PI
#define M_PI    3.14159265358979323846f
#endif

//double log2(double d) {return log(d)/log(2) ;} // uncomment this on <C++11

#ifndef INCHTOM
#define INCHTOM(b) (b*39.3701)
#endif

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

PositionalAudio::PositionalAudio(Plugin& plugin)
    : m_talkers(plugin.talkers())
    , m_info_data(plugin.info_data())
    , m_servers_info(plugin.servers_info())
    , universe(new TsVrUniverse(this))
    , meObj(new TsVrObjSelf(this))
    , m_websockets_server(plugin.websocket_server())
{
    setParent(&plugin);
    setObjectName("PositionalAudio");
    m_isPrintEnabled = false;
    
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

bool PositionalAudio::isUseAttenuation() const
{
    return m_isUseAttenuation;
}

int PositionalAudio::getDistanceMin() const
{
    return m_distanceMin;
}

int PositionalAudio::getDistanceMax() const
{
    return m_distanceMax;
}

float PositionalAudio::getRollOff() const
{
    return m_rollOff;
}

float PositionalAudio::getRollOffMax() const
{
    return m_rollOffMax;
}

bool PositionalAudio::isPositioned(anyID clientID) const
{
    return m_PlayersInMyContext.contains(clientID);
}

void PositionalAudio::setUseCamera(bool val)
{
    if (m_isUseCamera == val)
        return;
    m_isUseCamera = val;
    emit useCameraChanged(m_isUseCamera);
}

void PositionalAudio::setUseAttenuation(bool val)
{
    if (m_isUseAttenuation == val)
        return;
    m_isUseAttenuation = val;
    emit useAttenuationChanged(m_isUseAttenuation);
}

void PositionalAudio::setDistanceMin(int val)
{
    if (m_distanceMin == val)
        return;
    m_distanceMin = val;
    emit distanceMinChanged(m_distanceMin);
}

void PositionalAudio::setDistanceMax(int val)
{
    if (m_distanceMax == val)
        return;
    m_distanceMax = val;
    emit distanceMaxChanged(m_distanceMax);
}

void PositionalAudio::setRollOff(float val)
{
    if (m_rollOff == val)
        return;
    m_rollOff = val;
    //m_rollOff_Lin = db2lin_alt2(m_rollOff);
    emit rollOffChanged(m_rollOff);
}

void PositionalAudio::setRollOffMax(float val)
{
    if (m_rollOffMax == val)
        return;
    m_rollOffMax = val;
    m_rollOffMax_Lin = db2lin_alt2(m_rollOffMax);
    emit rollOffMaxChanged(m_rollOffMax);
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
    Log(QString("(RemoveServerSetting) %1").arg(serverName), LogLevel_INFO);
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
//    Log(QString("Block Status Changed: %1 %2").arg(m_ServerSettings.value(serverUniqueId).serverName).arg((m_ServerSettings.value(serverUniqueId).isBlocked)?"blocked":"unblocked"));
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
    if (val.isEmpty())
        emit BroadcastJSON(QStringLiteral("{\"me\":true}"));

    emit myVrChanged(val);
}

void PositionalAudio::onMyIdentityChanged(TsVrObj *obj, QString val)
{
    Q_UNUSED(obj);
    emit myIdentityChanged(val);
}

void PositionalAudio::onUniverseRemoved(QString clientUID)
{
    QString out_stri;
    QTextStream out(&out_stri);
    out << "{";
    out << "\"uid\":\"" << clientUID << "\",";
    out << "\"me\":false}";
    emit BroadcastJSON(out_stri);
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
    if ((!isRunning()) || (meObj->getVr().isEmpty()))
        return;

    if (!m_isUseAttenuation)
        *volume = 1.0f;
    else
    {
        if (!m_PlayersInMyContext.contains(serverConnectionHandlerID,clientID))
            *volume = 1.0f;
        else
        {
            if ((m_distanceMax > 0) && (distance >= m_distanceMax))
                *volume = 0.0f;
            else if (distance <= m_distanceMin)
                *volume = 1.0f;
            else
            {
                distance = distance - m_distanceMin;
                if(distance <= 1)
                    *volume = 1.0f;
                else
                    *volume = db2lin_alt2(log2(distance) * m_rollOff);

                if (*volume < m_rollOffMax_Lin)
                    *volume = m_rollOffMax_Lin;
            }
        }
    }
}

bool PositionalAudio::onInfoDataChanged(uint64 serverConnectionHandlerID, uint64 id, PluginItemType type, uint64 mine, QTextStream &data)
{
    if (!isRunning())
        return false;

    auto is_dirty = false;
    if (type == PLUGIN_CLIENT)
    {
        if (id == mine)
            is_dirty |= meObj->onInfoDataChanged(data);
        else if (universe->Contains(serverConnectionHandlerID,(anyID)id))
            is_dirty |= universe->Get(serverConnectionHandlerID,(anyID)id)->onInfoDataChanged(data);
        else
            return is_dirty;

        if (is_dirty && (id != mine))
        {
            if (m_PlayersInMyContext.contains(serverConnectionHandlerID,(anyID)id))
            {
                data << "\ncontext: match";
            }
            else
                data << "\ncontext: no match";

        }

    }
    return is_dirty;
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

    m_info_data.Register(this, value, 1);

    if (value)
    {
//        if (m_SendReturnCode.isEmpty())
//        {
//            m_SendReturnCodeC = new char[RETURNCODE_BUFSIZE];
//            ts3Functions.createReturnCode(pluginID,m_SendReturnCodeC,RETURNCODE_BUFSIZE);
//            m_SendReturnCode = QString::fromLatin1(m_SendReturnCodeC);
//            Log("Got returnCode: " + m_SendReturnCode);
//        }
        connect(&m_talkers, &Talkers::ConnectStatusChanged, this, &PositionalAudio::onConnectStatusChanged, Qt::UniqueConnection);
        connect(universe, &TsVrUniverse::removed, this, &PositionalAudio::onUniverseRemoved, Qt::UniqueConnection);
        connect(meObj, &TsVrObjSelf::vrChanged, this, &PositionalAudio::onMyVrChanged, Qt::UniqueConnection);
        connect(meObj, &TsVrObjSelf::identityChanged, this, &PositionalAudio::onMyIdentityChanged, Qt::UniqueConnection);

        connect(this, &PositionalAudio::BroadcastJSON, &m_websockets_server, &ServerThreaded::broadcastMessage, Qt::UniqueConnection);

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
        disconnect(&m_talkers, &Talkers::ConnectStatusChanged, this, &PositionalAudio::onConnectStatusChanged);
        disconnect(universe, &TsVrUniverse::removed, this, &PositionalAudio::onUniverseRemoved);
        disconnect(meObj, &TsVrObjSelf::vrChanged, this, &PositionalAudio::onMyVrChanged);
        disconnect(meObj, &TsVrObjSelf::identityChanged, this, &PositionalAudio::onMyIdentityChanged);
        unlock();
        this->killTimer(m_tryTimerId);
        lm = NULL;
        m_sharedMemory->detach();
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
                m_info_data.RequestSelfUpdate();
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
                    auto vr_desc = QString::fromWCharArray(lm->description, 2048);
                    const auto kNullPos = vr_desc.indexOf(QChar::Null, 0);
                    if (kNullPos == -1)
                        Error("(UpdateMyGame) game description Null Terminator not found.");
                    else if (kNullPos != 256)
                        vr_desc.truncate(kNullPos);

                    meObj->setVrDescription(vr_desc);
                }
                else
                    meObj->setVrDescription(QString::null);

                if (lm->name[0])
                {
                    auto my_vr = QString::fromWCharArray(lm->name, 256);
                    const auto kNullPos = my_vr.indexOf(QChar::Null, 0);
                    if (kNullPos == -1)
                        Error("(UpdateMyGame) game Null Terminator not found.");
                    else if (kNullPos != 256)
                        my_vr.truncate(kNullPos);

                    meObj->setVr(my_vr);

                    if (!my_vr.isEmpty())
                    {
                        // Log("Found " + meObj->getVr());
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
    m_IdentityUncleaned.clear();

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
    
    if (m_fetchTimerId)
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

    meObj->setIdentityRaw(QString::null);
    meObj->setContext(QString::null);
    meObj->setVr(QString::null);
    meObj->setVrDescription(QString::null);

    Send(QString::null, PluginCommandTarget_CURRENT_CHANNEL);
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
//        Log(QString("fetch: elapsed: %1 timeout at: %2").arg(m_fetchTimerElapsed).arg(m_fetchTimerTimeoutMsec));
        m_sharedMemory->unlock();
        return !(m_fetchTimerElapsed > m_fetchTimerTimeoutMsec);    // -> unlock on false
    }
    m_fetchTimerElapsed = 0;
    m_lastCount = lm->dwcount;

//    m_Avatar_Dirty = (m_Avatar_Dirty || !((meObj->getAvatarPosition() == lm->fAvatarPosition) && (meObj->getAvatarFront() == lm->fAvatarFront) && (meObj->getAvatarTop() == lm->fAvatarTop)));

    m_Avatar_Dirty = (meObj->setAvatar(lm->fAvatarPosition,lm->fAvatarFront,lm->fAvatarTop) || m_Avatar_Dirty);

    if (lm->uiVersion == 2)
    {
        //*** Camera
        meObj->setCamera(lm->fCameraPosition,lm->fCameraFront,lm->fCameraTop);

        if (lm->context_len > 255)
            lm->context_len = 255;

        //*** Context
        auto newContext = QByteArray(reinterpret_cast<const char *>(lm->context), lm->context_len);
        m_Context_Dirty = (m_Context != newContext);
        m_Context = newContext;
        if (m_Context_Dirty)
            meObj->setContext(m_Context.toHex().data());    //m_ContextHex = m_Context.toHex().data();

        //*** Identity
        auto new_identity = QString::fromWCharArray(lm->identity, 256);
        m_isDirty_IdentityUncleaned = (m_IdentityUncleaned != new_identity);
        if (m_isDirty_IdentityUncleaned)
        {
            m_IdentityUncleaned = new_identity;
            const auto kNullPos = new_identity.indexOf(QChar::Null, 0);
            if (kNullPos == -1)
                Error("identity Null Terminator not found.");
            else if (kNullPos != 256)
                new_identity.truncate(kNullPos);

            meObj->setIdentityRaw(new_identity);
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
            //Log("I left vr.",LogLevel_INFO);
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
            auto args_stri = args.readAll().trimmed();
            auto list = args_stri.split("[Ct_Delimiter]",QString::KeepEmptyParts,Qt::CaseSensitive);    // keep empty parts?
            const auto& name = list.at(0);
            if (list.size() > 1)
            {
                auto rest = list.at(1);
                QTextStream in(&rest);

                QString context;
                in >> context;

                auto identity = in.readAll().trimmed();

                //Log(QString("Received (me): cId: %1 VR: %2 CO: %3 ID: %4").arg(clientID).arg(name).arg((context == meObj->getContext())?"match":"no match -.-").arg(identity), serverConnectionHandlerID, LogLevel_DEBUG);
            }
            else    //version 1
            {
                //Log(QString("Received (me): cId: %1 VR: %2").arg(clientID).arg(name), serverConnectionHandlerID, LogLevel_DEBUG);
            }
        }
//        else
//            Log("Received(me) AV",serverConnectionHandlerID,LogLevel_DEBUG);

        return true;
    }

    auto obj = universe->Get(serverConnectionHandlerID,clientID);
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
        auto args_stri = args.readAll().trimmed();
        auto list = args_stri.split("[Ct_Delimiter]",QString::KeepEmptyParts,Qt::CaseSensitive);    // keep empty parts?
        auto name = list.at(0);

        auto isDirtyName = (name != obj->getVr());
        obj->setVr(name);
        auto isDirtyContext = false;
        auto isDirtyId = false;

        if (list.size() > 1)
        {
            auto rest = list.at(1);
            QTextStream in(&rest);

            QString context;
            in >> context;
            isDirtyContext = (context != obj->getContext());
            obj->setContext(context);

            auto identity = in.readAll().trimmed();
            isDirtyId = (identity != obj->getIdentityRaw());
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
            //Log(QString("Received: cId: %1 VR: %2").arg(clientID).arg(name), serverConnectionHandlerID, LogLevel_DEBUG);
        }

        if (isDirtyName || isDirtyContext || isDirtyId)
            m_info_data.RequestUpdate(serverConnectionHandlerID,clientID);
    }

    if (m_PlayersInMyContext.contains(serverConnectionHandlerID,clientID))
    {
        auto vector = obj->getAvatarPosition();
        ts3Functions.channelset3DAttributes(serverConnectionHandlerID,clientID,&vector);
    }

    auto sendString = GetSendStringJson(true,false,obj);
    emit BroadcastJSON(sendString);
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
            auto myContext = meObj->getContext();
            out << "[Ct_Delimiter]" << (myContext.isEmpty()?"[Ct_None]":myContext);
            auto my_ident = meObj->getIdentityRaw();
            if (!my_ident.isEmpty())
                out << " " << my_ident;
        }
    }
    return out_stri;
}

QString PositionalAudio::GetSendStringJson(bool isAll, bool isMe, TsVrObj* obj)
{
    if (isMe)
        obj = meObj;

    QString out_stri;
    QTextStream out(&out_stri);
    out << "{";
    auto vec = obj->getAvatarPosition();
    out << "\"px\":" << INCHTOM(vec.x) << "," << "\"pz\":" << INCHTOM(vec.z) << ",";// << "\"ap_z\":" << vec.z << ",";

    vec = obj->getAvatarFront();

    int front = atan2(vec.z, vec.x)*180/M_PI;
    if (front <0)
        front += 360;

    front *= -1;
    out << "\"pa\":" << front << ",";


    if (isAll)
    {
        auto ident = obj->getIdentityRaw();
        if (ident.isEmpty())
        {
            Log("ident is empty!",LogLevel_INFO);   //fixed with >1.5.0
            return QString::null;
        }

        ident.chop(1);
        ident.remove(0,1);
        out << ident << ",";

        if (!isMe)
        {
            auto iObj = qobject_cast<TsVrObjOther *>(obj);
            if (iObj)
            {
                unsigned int error;
                char name[512];
                auto serverConnectionHandlerID = iObj->getServerConnectionHandlerID();
                auto clientID = iObj->getClientID();
                if((error = ts3Functions.getClientDisplayName(serverConnectionHandlerID, clientID, name, 512)) != ERROR_ok)
                    Error("(GetSendStringJson)",serverConnectionHandlerID,error);
                else
                    out << "\"vcname\":\"" << name << "\",";

                out << "\"uid\":\"" << iObj->getClientUID() << "\",";
            }
        }
    }
    else
        Error("not isAll!");

    out << "\"me\":" << ((isMe)?"true":"false") << "}";
    return out_stri;
}

//! Non-throttled DoSend
void PositionalAudio::Send(uint64 serverConnectionHandlerID, QString args, int targetMode, const anyID *targetIDs, const char *returnCode)
{
    QString cmd;
    QTextStream str(&cmd);
    if (args.isEmpty())
        str << "3D";   // left game (unlocked)
    else
        str << "3D " << args;

//    Log(QString("Sending: %1").arg(cmd),serverConnectionHandlerID,LogLevel_DEBUG);
//    returnCode = m_SendReturnCodeC;
    auto plugin = qobject_cast<Plugin_Base*>(parent());
    ts3Functions.sendPluginCommand(serverConnectionHandlerID, plugin->id().c_str(), cmd.toLatin1().constData(), targetMode, targetIDs, returnCode);
}

void PositionalAudio::Send(QString args, int targetMode)
{
    if (!m_ServerSettings.contains("default"))
        return;

    // Get clients in my context
    uint64* servers;
    if (ts3Functions.getServerConnectionHandlerList(&servers) == ERROR_ok)
    {
//        m_silentSendCounter++;

        auto myTalkingScHandler = m_talkers.isMeTalking();

        for (auto server = servers; *server; ++server)
        {
            auto server_info = m_servers_info.get_server_info(*server);
            if (!server_info)
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

            auto sUId = server_info->getUniqueId();
            const auto s_settings = (m_ServerSettings.contains(sUId)?m_ServerSettings.value(sUId):m_ServerSettings.value("default"));
            if (!s_settings.enabled)
                continue;

            if (targetMode == PluginCommandTarget_CLIENT)
            {
                if ((m_ServerSettings.contains(sUId)) && m_ServerSettings.value(sUId).isBlocked)    //don't fallback on "default", which should never be isBlocked
                    continue;

                int count_max = (s_settings.sendInterval) * SEND_INTERVAL_MODIFIER; // with SEND_THROTTLE_GLOBAL == 5 normalized; todo dynamic
                if (myTalkingScHandler != *server)
                {
//                    Log("Adding silent interval increase");
                    count_max += (s_settings.sendIntervalSilentInc * SEND_INTERVAL_MODIFIER);
                }

                if (m_SendCounters[*server]++ < count_max)
                    continue;

//                Log(QString("count: %1; max count: %2").arg(m_SendCounters[*server]+1).arg(count_max));
                m_SendCounters[*server] = 0;

                /*QVector<anyID> vec = QVector<anyID>::fromList(m_PlayersInMyContext.values(*server));
                if (!vec.isEmpty())
                {
                    // For testing purposes
                    anyID myID;
                    unsigned int error;
                    if ((error = ts3Functions.getClientID(*server,&myID)) == ERROR_ok)
                        vec.append(myID);

                    vec.append((anyID)0);
                    Send(*server, args, targetMode, vec.constData(), NULL);
                }*/
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
            Send(args, PluginCommandTarget_CURRENT_CHANNEL);

            args = GetSendStringJson(true, true, NULL);
            if (!args.isEmpty())
                emit BroadcastJSON(args);
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
                args = GetSendStringJson(true,true, NULL);
                if (!args.isEmpty())
                    emit BroadcastJSON(args);
            }
        }
    }
}

void PositionalAudio::Update3DListenerAttributes()
{
    uint64* servers;
    if(ts3Functions.getServerConnectionHandlerList(&servers) == ERROR_ok)
    {
        auto myVr = meObj->getVr();
        auto myContext = meObj->getContext();
        for(auto server = servers; *server != (uint64)NULL; ++server)
        {
            int status;
            if (ts3Functions.getConnectionStatus(*server, &status) != ERROR_ok)
                continue;

            if (status != STATUS_CONNECTION_ESTABLISHED)
                continue;

            TS3_VECTOR pos   = m_isUseCamera ? meObj->getCameraPosition() : meObj->getAvatarPosition();
            TS3_VECTOR front = m_isUseCamera ? meObj->getCameraFront() : meObj->getAvatarFront();
            TS3_VECTOR top   = m_isUseCamera ? meObj->getCameraTop() : meObj->getAvatarTop();

            ts3Functions.systemset3DListenerAttributes(*server,&pos,&front,&top);


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
                                    auto obj = universe->Get(*server, clients[i]);
                                    if ((myVr == obj->getVr()) && (myContext == obj->getContext()))
                                        m_PlayersInMyContext.insert(*server, clients[i]);
                                }
                            }

                            if (!m_PlayersInMyContext.contains(*server,clients[i]))
                                ts3Functions.channelset3DAttributes(*server,clients[i],&pos);
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
