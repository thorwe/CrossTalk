/*
 * Obviously, the core of this needs to have a certain amount of similarities to Mumble's Link plugin,
 * @ https://sourceforge.net/p/mumble/code/ci/master/tree/plugins/link/
 * which is under what appears to be a 3-clause BSD licence also operating under the alias of modified BSD
 * license
 *
 */
#include "mod_positionalaudio.h"

#include "linked_mem.h"

#include "core/ts_functions.h"
#include "core/ts_helpers_qt.h"
#include "core/ts_serversinfo.h"

#include "teamspeak/public_rare_definitions.h"

#include "plugin.h"

#include "volume/db.h"

#include "gsl/gsl_util"

#include <cstdint>
#include <tuple>

constexpr const int kFetchTimerInterval = 20;  // Mumble fetches these values 50 times a second aka 20msec
constexpr const int kSendThrottleGlobal =
5;  // Send(): 50times/sec; count to 5 -> 10times/sec; save some computation
constexpr const int kSendIntervalModifier = 50 / kSendThrottleGlobal;  // I...think.

#ifndef M_PI
#define M_PI 3.14159265358979323846f
#endif

// double log2(double d) {return log(d)/log(2) ;} // uncomment this on <C++11

#ifndef INCHTOM
#define INCHTOM(b) (b * 39.3701)
#endif

namespace thorwe
{

PositionalAudio::PositionalAudio(Plugin &plugin)
    : m_talkers(plugin.talkers())
    , m_info_data(plugin.info_data())
    , m_servers_info(plugin.servers_info())
    , m_websockets_server(plugin.websocket_server())
{
    setParent(&plugin);
    setObjectName("PositionalAudio");
    m_isPrintEnabled = false;
}

// Properties
std::wstring PositionalAudio::get_my_vr() const
{
    return meObj.get_vr();
}

std::wstring PositionalAudio::getMyIdentity() const
{
    return meObj.getIdentity();
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

bool PositionalAudio::isPositioned(anyID client_id) const
{
    return m_PlayersInMyContext.contains(client_id);
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
    // m_rollOff_Lin = db2lin_alt2(m_rollOff);
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
    m_ServerSettings.insert(serverUniqueId, setting);
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
        QMapIterator<QString, PositionalAudio_ServerSettings> i(m_ServerSettings);
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
    //    Log(QString("Block Status Changed: %1
    //    %2").arg(m_ServerSettings.value(serverUniqueId).serverName).arg((m_ServerSettings.value(serverUniqueId).isBlocked)?"blocked":"unblocked"));
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

void PositionalAudio::onMyIdentityChanged(TsVrObj *obj, QString val)
{
    Q_UNUSED(obj);
    emit myIdentityChanged(val);
}

void PositionalAudio::remove_other(connection_id_t connection_id, client_id_t client_id)
{
    const auto [was_removed, client_uid] = m_universe.remove(connection_id, client_id);
    QString out_stri;
    QTextStream out(&out_stri);
    out << "{";
    out << R"("uid":")" << QString::fromStdString(client_uid) << "\",";
    out << "\"me\":false}";
    emit BroadcastJSON(out_stri);
}

QMap<QString, PositionalAudio_ServerSettings> PositionalAudio::getServerSettings() const
{
    return m_ServerSettings;
}

/*bool PositionalAudio::RegisterCustomEnvironmentSupport(QObject *p)
{
    CustomEnvironmentSupportInterface *iCustomEnvironmentSupport =
qobject_cast<CustomEnvironmentSupportInterface *>(p); if (!iCustomEnvironmentSupport) return false;

    m_CustomEnvironmentSupportMap.insert(p->objectName(),p);
    return true;
}*/

// Other
void PositionalAudio::onClientMoveEvent(
uint64 connection_id, anyID client_id, uint64 oldChannelID, uint64 newChannelID, int visibility, anyID my_id)
{
    Q_UNUSED(newChannelID);

    if (client_id == my_id)  // && newChannelID != 0 should be filtered out on plugin level already
    {
        QString args = GetSendString(true);
        if (!args.isEmpty())
            Send(connection_id, args, PluginCommandTarget_CURRENT_CHANNEL, {});
    }
    else
    {
        bool isRemove = (visibility == LEAVE_VISIBILITY);
        if (!isRemove)
        {
            const auto [error_channel_of_client, channel_id] =
            com::teamspeak::pluginsdk::funcs::get_channel_of_client(connection_id, my_id);
            if (error_channel_of_client)
                Error("(onClientMoveEvent)", connection_id, error_channel_of_client);
            else
                isRemove = (oldChannelID == channel_id);  // leave without losing visibility
        }
        if (isRemove)
        {
            remove_other(connection_id, client_id);
            if (m_PlayersInMyContext.contains(connection_id, client_id))
            {
                m_PlayersInMyContext.remove(connection_id, client_id);
                if (const auto error_channel_set_3d =
                    com::teamspeak::pluginsdk::funcs::sound::channelset_3d_attributes(connection_id,
                                                                                      client_id, NULL_VECTOR);
                    error_channel_set_3d)
                    Error("(onClientMoveEvent)", connection_id, error_channel_set_3d);
            }
        }
    }
}

void PositionalAudio::onCustom3dRolloffCalculationClientEvent(uint64 connection_id,
                                                              anyID client_id,
                                                              float distance,
                                                              float *volume)
{
    if ((!running()) || (meObj.get_vr().empty()))
        return;

    if (!m_isUseAttenuation)
        *volume = 1.0f;
    else
    {
        if (!m_PlayersInMyContext.contains(connection_id, client_id))
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
                if (distance <= 1)
                    *volume = 1.0f;
                else
                    *volume = db2lin_alt2(log2(distance) * m_rollOff);

                if (*volume < m_rollOffMax_Lin)
                    *volume = m_rollOffMax_Lin;
            }
        }
    }
}

bool PositionalAudio::onInfoDataChanged(
uint64 connection_id, uint64 id, PluginItemType type, uint64 mine, QTextStream &data)
{
    if (!running())
        return false;

    bool is_dirty = false;
    if (type == PLUGIN_CLIENT)
    {
        if (id == mine)
            is_dirty |= meObj.onInfoDataChanged(data);
        else
        {
            const auto [found, info_result] = m_universe.do_for(
            [&data](TsVrObjOther *obj) {
                if (!obj)
                    return std::make_tuple<bool, bool>(false, false);

                return std::make_tuple<bool, bool>(true, obj->onInfoDataChanged(data));
            },
            connection_id, static_cast<client_id_t>(id));
            if (!found)
                return is_dirty;

            is_dirty = is_dirty || info_result;
        }

        if (is_dirty && (id != mine))
        {
            if (m_PlayersInMyContext.contains(connection_id, (anyID) id))
            {
                data << "\ncontext: match";
            }
            else
                data << "\ncontext: no match";
        }
    }
    return is_dirty;
}

/*int PositionalAudio::onServerErrorEvent(uint64 connection_id, const char *errorMessage, unsigned int error,
const char *returnCode, const char *extraMessage)
{
    Q_UNUSED(errorMessage);
    Q_UNUSED(returnCode);
    Q_UNUSED(extraMessage);

    if (isRunning() && (error==ERROR_client_is_flooding))    // check if im sending there once this is
implemented
    {
        QString sUId = TSServersInfo::instance()->GetServerInfo(connection_id)->getUniqueId();
        emit serverBlock(sUId);
    }

    return 0;
}*/

void PositionalAudio::onRunningStateChanged(bool value)
{
    m_isDirty_IdentityUncleaned = false;  // not value, we want it to be off no matter what
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

        connect(this, &PositionalAudio::BroadcastJSON, &m_websockets_server,
                &ServerThreaded::broadcastMessage, Qt::UniqueConnection);

        m_shared_memory = new QSharedMemory(this);
        m_shared_memory->setNativeKey("MumbleLink");
        if (!m_shared_memory->create(sizeof(LinkedMem), QSharedMemory::ReadWrite))
        {
            if (m_shared_memory->error() == QSharedMemory::AlreadyExists)
            {
                if (!m_shared_memory->attach(QSharedMemory::ReadWrite))
                {
                    Error(
                    QString("Could not attach to shared memory: %1").arg(m_shared_memory->errorString()));
                    return;
                }
            }
            else
            {
                Error(QString("Could not create shared memory: %1").arg(m_shared_memory->errorString()));
                return;
            }
        }
        else
            memset(m_shared_memory->data(), 0, m_shared_memory->size());

        auto *lm = static_cast<LinkedMem *>(m_shared_memory->data());
        if (!lm)
        {
            m_shared_memory->detach();
            Error("Could not cast shared memory to struct.");
            return;
        }
        m_shared_mem_reader.set_lm(lm);
        m_tryTimerId = this->startTimer(1000);
    }
    else
    {
        unlock();
        this->killTimer(m_tryTimerId);
        m_shared_mem_reader.set_lm(nullptr);
        m_shared_memory->detach();
    }
    Log(QString("enabled: %1").arg((value) ? "true" : "false"));
}

void PositionalAudio::timerEvent(QTimerEvent *event)
{
    if (event->timerId() == m_fetchTimerId)
    {
        if (fetch())
        {
            if (m_fetchTimerElapsed != 0)  // bool isFetchDirt =
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
    else if (event->timerId() == m_tryTimerId)  // "trylock" in Mumble plugin API style
    {
        m_shared_memory->lock();
        const auto maybe_diff = m_shared_mem_reader.try_read();
        m_shared_memory->unlock();

        if (maybe_diff.has_value())
        {
            const auto &diff = maybe_diff.value();
            meObj.set_vr_description(diff.description);
            set_my_vr(diff.name);

            const auto my_vr = get_my_vr();
            if (!my_vr.empty())
            {
                // Log("Found " + meObj.getVr());
                if (m_tryTimerId != 0)
                {
                    this->killTimer(m_tryTimerId);
                    m_tryTimerId = 0;
                }
                m_lastCount = m_shared_mem_reader.dw_count();
                m_fetchTimerElapsed = 0;
                m_fetchTimerId = this->startTimer(kFetchTimerInterval);
            }
        }
    }
}

void PositionalAudio::unlock()
{
    m_isDirty_IdentityUncleaned = false;

    {
        m_shared_memory->lock();
        m_shared_mem_reader.zero_lm();
        m_shared_memory->unlock();
    }
    m_lastCount = 0;

    if (m_fetchTimerId)
    {
        killTimer(m_fetchTimerId);
        m_fetchTimerId = 0;
    }
    if (running() && m_tryTimerId == 0)
        m_tryTimerId = this->startTimer(1000);

    meObj.reset_avatar();
    meObj.reset_camera();

    m_PlayersInMyContext.clear();
    Update3DListenerAttributes();

    set_my_identity();
    meObj.set_context({});
    set_my_vr();
    meObj.set_vr_description();

    Send(QString::null, PluginCommandTarget_CURRENT_CHANNEL);
    Log("Unlocked.");
}

bool PositionalAudio::fetch()
{
    m_shared_memory->lock();
    const auto maybe_diff = m_shared_mem_reader.read_mem();
    if (maybe_diff.has_value())
    {
        const auto version = maybe_diff.value().ui_version;
        if (version != 1 && version != 2)
            return false;  // -> unlock
    }
    else
    {
        m_fetchTimerElapsed = m_fetchTimerElapsed + kFetchTimerInterval;
        m_shared_memory->unlock();
        return !(m_fetchTimerElapsed > m_fetchTimerTimeoutMsec);  // -> unlock on false
    }
    m_fetchTimerElapsed = 0;

    const auto &diff = maybe_diff.value();
    m_lastCount = diff.dw_count;

    if (diff.avatar.has_value())
        meObj.set_avatar(diff.avatar.value());

    m_Avatar_Dirty = diff.avatar.has_value() || m_Avatar_Dirty;

    if (diff.ui_version == 2)
    {
        if (diff.camera.has_value())
            meObj.set_camera(diff.camera.value());

        // TODO: why did I do that back then?
        /*if (lm->context_len > 255)
            lm->context_len = 255;*/

        //*** Context
        m_Context_Dirty = diff.context.has_value();
        if (m_Context_Dirty)
            meObj.set_context(diff.context.value());

        //*** Identity
        m_isDirty_IdentityUncleaned = diff.identity.has_value();
        if (m_isDirty_IdentityUncleaned)
            set_my_identity(diff.identity.value());
    }
    else if (diff.ui_version == 1)  // legacy
    {
        if (diff.avatar.has_value())  // we feed the avatar to the camera
            meObj.set_camera(diff.avatar.value());

        m_Context_Dirty = false;
        meObj.set_context({});
        m_isDirty_IdentityUncleaned = false;
        set_my_identity();
    }

    m_shared_memory->unlock();
    return true;
}

bool PositionalAudio::onPluginCommand(
uint64 connection_id, anyID client_id, bool isMe, QString cmd, QTextStream &args)
{
    if (cmd != "3D")
        return false;

    if (isMe)
    {
        if (args.atEnd())
        {
            // Log("I left vr.",LogLevel_INFO);
            return true;
        }

        TS3_VECTOR myAvatarPosition;
        TS3_VECTOR myAvatarFront;
        TS3_VECTOR myAvatarTop;
        args >> myAvatarPosition >> myAvatarFront >> myAvatarTop;

        if (!args.atEnd())
        {
            // name
            // context
            // identity
            auto args_stri = args.readAll().trimmed();
            auto list = args_stri.split("[Ct_Delimiter]", QString::KeepEmptyParts,
                                        Qt::CaseSensitive);  // keep empty parts?
            // const auto &name = list.at(0);
            if (list.size() > 1)
            {
                auto rest = list.at(1);
                QTextStream in(&rest);

                QString context;
                in >> context;

                auto identity = in.readAll().trimmed();

                // Log(QString("Received (me): cId: %1 VR: %2 CO: %3 ID:
                // %4").arg(client_id).arg(name).arg((context == meObj.getContext())?"match":"no match
                // -.-").arg(identity), connection_id, LogLevel_DEBUG);
            }
            else  // version 1
            {
                // Log(QString("Received (me): cId: %1 VR: %2").arg(client_id).arg(name), connection_id,
                // LogLevel_DEBUG);
            }
        }
        //        else
        //            Log("Received(me) AV",connection_id,LogLevel_DEBUG);

        return true;
    }

    const auto [obj, added] = m_universe.add_obj(connection_id, client_id);
    if (added)
        m_IsSendAllOverride = true;

    if (args.atEnd())  // Player left vr (unlocked)
    {
        Log(QString("%1 left %2 VR.").arg(obj->getIdentity()).arg(QString::fromStdWString(obj->get_vr())));
        remove_other(connection_id, client_id);
        m_PlayersInMyContext.remove(connection_id, client_id);
        return true;
    }

    // args >> obj->avatarPosition >> obj->avatarFront >> obj->avatarTop;
    TS3_VECTOR avatarPosition;
    TS3_VECTOR avatarFront;
    TS3_VECTOR avatarTop;
    args >> avatarPosition >> avatarFront >> avatarTop;
    obj->set_avatar({avatarPosition, avatarFront, avatarTop});

    if (!args.atEnd())
    {
        // name
        // context
        // identity
        auto args_stri = args.readAll().trimmed();
        auto list =
        args_stri.split("[Ct_Delimiter]", QString::KeepEmptyParts, Qt::CaseSensitive);  // keep empty parts?
        auto name = list.at(0).toStdWString();

        auto isDirtyName = (name != obj->get_vr());
        obj->set_vr(name);
        auto isDirtyContext = false;
        auto isDirtyId = false;

        if (list.size() > 1)
        {
            auto rest = list.at(1);
            QTextStream in(&rest);

            auto context = std::vector<std::byte>{};
            {
                QString context_q;
                in >> context_q;
                const auto context_s = context_q.toStdString();
                context.resize(context_s.size());
                std::copy_n(reinterpret_cast<const std::byte *>(context_s.data()), context_s.size(),
                            context.data());
                isDirtyContext = (context != obj->get_context());
                obj->set_context(context);
            }

            auto identity = in.readAll().trimmed().toStdWString();
            isDirtyId = (identity != obj->getIdentityRaw());
            obj->setIdentityRaw(identity);

            Log(QString("Received: VR: %2 CO: %3 ID: %4")
                .arg(name)
                .arg((context == meObj.get_context()) ? "match" : "no match")
                .arg(identity),
                connection_id, LogLevel_DEBUG);

            if (isDirtyName || isDirtyContext)
            {
                if (meObj.get_vr() == obj->get_vr())
                {
                    if (context == meObj.get_context())
                    {
                        if (!m_PlayersInMyContext.contains(connection_id, client_id))
                            m_PlayersInMyContext.insert(connection_id, client_id);
                    }
                    else if (m_PlayersInMyContext.contains(connection_id, client_id))
                        m_PlayersInMyContext.remove(connection_id, client_id);
                }
                else
                    m_PlayersInMyContext.remove(connection_id, client_id);
            }
        }
        else  // version 1
        {
            // Log(QString("Received: cId: %1 VR: %2").arg(client_id).arg(name), connection_id,
            // LogLevel_DEBUG);
        }

        if (isDirtyName || isDirtyContext || isDirtyId)
            m_info_data.RequestUpdate(connection_id, client_id);
    }

    if (m_PlayersInMyContext.contains(connection_id, client_id))
    {
        const auto &avatar = obj->get_avatar();
        com::teamspeak::pluginsdk::funcs::sound::channelset_3d_attributes(connection_id, client_id,
                                                                          avatar.position);
    }

    auto sendString = GetSendStringJson(true, false, obj);
    emit BroadcastJSON(sendString);
    return true;
}

QString PositionalAudio::GetSendString(bool isAll)
{
    // Bulk version
    if (meObj.get_vr().empty())
        return QString::null;

    const auto &avatar = meObj.get_avatar();
    QString out_stri;
    QTextStream out(&out_stri);
    out << avatar.position << avatar.front << avatar.top;
    if (isAll)
    {
        //        out << " " << m_GameName;
        out << " " << meObj.get_vr();

        {
            const auto &my_context = meObj.get_context_as_string();
            const auto &my_ident = meObj.getIdentityRaw();
            if (!(my_context.empty() && my_ident.empty()))  // indicates uiVersion 1
            {
                out << "[Ct_Delimiter]"
                    << (my_context.empty() ? "[Ct_None]" : QString::fromStdString(my_context));

                if (!my_ident.empty())
                    out << " " << my_ident;
            }
        }
    }
    return out_stri;
}

QString PositionalAudio::GetSendStringJson(bool isAll, bool isMe, TsVrObj *obj)
{
    using namespace com::teamspeak::pluginsdk;

    if (isMe)
        obj = &meObj;

    QString out_stri;
    QTextStream out(&out_stri);
    out << "{";
    const auto &avatar = obj->get_avatar();
    out << "\"px\":" << INCHTOM(avatar.position.x) << ","
        << "\"pz\":" << INCHTOM(avatar.position.z) << ",";  // << "\"ap_z\":" << vec.z << ",";

    int front = atan2(avatar.front.z, avatar.front.x) * 180 / M_PI;
    if (front < 0)
        front += 360;

    front *= -1;
    out << "\"pa\":" << front << ",";

    if (isAll)
    {
        auto ident = obj->getIdentityRaw();
        if (ident.empty())
        {
            Log("ident is empty!", LogLevel_INFO);  // fixed with >1.5.0
            return QString::null;
        }

        auto ident_q = QString::fromStdWString(ident);
        // TODO: why did I do that?
        ident_q.chop(1);
        ident_q.remove(0, 1);
        out << ident_q << ",";

        if (!isMe)
        {
            auto iObj = qobject_cast<TsVrObjOther *>(obj);
            if (iObj)
            {
                const auto connection_id = iObj->get_connection_id();
                const auto client_id = iObj->get_client_id();
                const auto [error_display_name, display_name] =
                funcs::get_client_display_name(connection_id, client_id);
                if (error_display_name)
                    Error("(GetSendStringJson)", connection_id, error_display_name);
                else
                    out << R"("vcname":")" << QString::fromUtf8(display_name.data()) << "\",";

                out << R"("uid":")" << QString::fromUtf8(iObj->get_client_uid().data()) << "\",";
            }
        }
    }
    else
        Error("not isAll!");

    out << "\"me\":" << ((isMe) ? "true" : "false") << "}";
    return out_stri;
}

//! Non-throttled DoSend
void PositionalAudio::Send(uint64 connection_id,
                           const QString &args,
                           int target_mode,
                           const std::vector<anyID> &target_ids)
{
    QString cmd;
    QTextStream str(&cmd);
    if (args.isEmpty())
        str << "3D";  // left game (unlocked)
    else
        str << "3D " << args;

    //    Log(QString("Sending: %1").arg(cmd),connection_id,LogLevel_DEBUG);
    auto plugin = qobject_cast<Plugin_Base *>(parent());
    com::teamspeak::pluginsdk::funcs::send_plugin_command(
    connection_id, plugin->id(), cmd.toLatin1().constData(), target_mode, target_ids);
}

void PositionalAudio::Send(const QString &args, int targetMode)
{
    using namespace com::teamspeak::pluginsdk;

    if (!m_ServerSettings.contains("default"))
        return;

    // Get clients in my context
    const auto [error_connection_ids, connection_ids] = funcs::get_server_connection_handler_ids();
    if (error_connection_ids)
        Error("(Send)", error_connection_ids);
    else
    {
        //        m_silentSendCounter++;

        const auto my_talking_connection_id = m_talkers.isMeTalking();

        for (const auto connection_id : connection_ids)
        {
            auto server_info = m_servers_info.get_server_info(connection_id);
            if (!server_info)
                continue;

            auto sUId = server_info->getUniqueId();
            const auto s_settings = (m_ServerSettings.contains(sUId) ? m_ServerSettings.value(sUId) :
                                                                       m_ServerSettings.value("default"));
            if (!s_settings.enabled)
                continue;

            if (targetMode == PluginCommandTarget_CLIENT)
            {
                if ((m_ServerSettings.contains(sUId)) &&
                    m_ServerSettings.value(sUId)
                    .isBlocked)  // don't fallback on "default", which should never be isBlocked
                    continue;

                int count_max =
                (s_settings.sendInterval) *
                kSendIntervalModifier;  // with kSendThrottleGlobal == 5 normalized; todo dynamic
                if (my_talking_connection_id != connection_id)
                {
                    //                    Log("Adding silent interval increase");
                    count_max += (s_settings.sendIntervalSilentInc * kSendIntervalModifier);
                }

                if (m_SendCounters[connection_id]++ < count_max)
                    continue;

                //                Log(QString("count: %1; max count:
                //                %2").arg(m_SendCounters[*server]+1).arg(count_max));
                m_SendCounters[connection_id] = 0;

                /*QVector<anyID> vec = QVector<anyID>::fromList(m_PlayersInMyContext.values(*server));
                if (!vec.isEmpty())
                {
                    // For testing purposes
                    anyID my_id;
                    unsigned int error;
                    if ((error = ts3Functions.getClientID(*server,&my_id)) == ERROR_ok)
                        vec.append(my_id);

                    vec.append((anyID)0);
                    Send(*server, args, targetMode, vec.constData(), NULL);
                }*/
                Send(connection_id, args, PluginCommandTarget_CURRENT_CHANNEL, {});
            }
            else
                Send(connection_id, args, targetMode, {});
        }
    }
    m_sendCounter = 0;
}

void PositionalAudio::Send()
{
    if (m_Context_Dirty || m_isDirty_IdentityUncleaned || m_IsSendAllOverride)
    {
        if (m_Context_Dirty)
            Log(QString("New context"));

        if (m_isDirty_IdentityUncleaned)
            Log("New identity: " + QString::fromStdWString(meObj.getIdentityRaw()));

        m_IsSendAllOverride = false;
        QString args = GetSendString(true);
        if (!args.isEmpty())
        {
            m_Avatar_Dirty = false;
            m_Context_Dirty = false;
            m_isDirty_IdentityUncleaned = false;
            Send(args, PluginCommandTarget_CURRENT_CHANNEL);

            args = GetSendStringJson(true, true, nullptr);
            if (!args.isEmpty())
                emit BroadcastJSON(args);
        }
    }
    else if (m_sendCounter > kSendThrottleGlobal)  // was: 50
    {
        m_silentSendCounter++;
        if (m_Avatar_Dirty)
        {
            QString args = GetSendString(false);
            if (!args.isEmpty())
            {
                m_Avatar_Dirty = false;
                Send(args, PluginCommandTarget_CLIENT);
                args = GetSendStringJson(true, true, nullptr);
                if (!args.isEmpty())
                    emit BroadcastJSON(args);
            }
        }
    }
}

void PositionalAudio::Update3DListenerAttributes()
{
    using namespace com::teamspeak::pluginsdk;
    const auto [error_connection_ids, connection_ids] = funcs::get_server_connection_handler_ids();
    if (!error_connection_ids)
    {
        const auto myVr = meObj.get_vr();
        const auto my_context = meObj.get_context();
        for (const auto connection_id : connection_ids)
        {
            if (const auto [error_connection_status, connection_status] =
                funcs::get_connection_status(connection_id);
                error_connection_status || connection_status != STATUS_CONNECTION_ESTABLISHED)
                continue;

            const auto &avatar_or_camera = m_isUseCamera ? meObj.get_camera() : meObj.get_avatar();

            if (const auto error_set_listener = funcs::sound::systemset_3d_listener_attributes(
                connection_id, avatar_or_camera.position, avatar_or_camera.front, avatar_or_camera.top);
                error_set_listener)
            {
                Error("(Update3DListenerAttributes)", connection_id, error_set_listener);
                continue;
            }

            const auto [error_my_id, my_id] = funcs::get_client_id(connection_id);
            if (error_my_id)
            {
                Error("(Update3DListenerAttributes)", connection_id, error_my_id);
                continue;
            }

            const auto [error_my_channel_id, my_channel_id] =
            funcs::get_channel_of_client(connection_id, my_id);
            if (error_my_channel_id)
            {
                Error("(Update3DListenerAttributes)", connection_id, error_my_channel_id);
                continue;
            }

            const auto [error_my_channel_clients, my_channel_clients] =
            funcs::get_channel_client_ids(connection_id, my_channel_id);
            if (error_my_channel_clients)
            {
                Error("(Update3DListenerAttributes)", connection_id, error_my_channel_clients);
                continue;
            }

            if (m_Context_Dirty)
                m_PlayersInMyContext.clear();

            for (const auto client_id : my_channel_clients)
            {
                if (m_Context_Dirty)  // Refill m_PlayersInMyContext
                {
                    m_universe.do_for(
                    [this, connection_id, client_id, &myVr, &my_context](TsVrObjOther *obj) {
                        if (!obj)
                            return;

                        if ((myVr == obj->get_vr()) && (my_context == obj->get_context()))
                            m_PlayersInMyContext.insert(connection_id, client_id);
                    },
                    connection_id, client_id);
                }

                if (!m_PlayersInMyContext.contains(connection_id, client_id))
                {
                    if (const auto error_channel_set_3d = funcs::sound::channelset_3d_attributes(
                        connection_id, client_id, avatar_or_camera.position);
                        error_channel_set_3d)
                        Error("(Update3DListenerAttributes)", connection_id, error_channel_set_3d);
                }
            }
        }
    }
}

void PositionalAudio::on_connect_status_changed(uint64 connection_id, int newStatus, unsigned int errorNumber)
{
    Q_UNUSED(errorNumber);

    if (!running())
        return;

    if (newStatus == STATUS_CONNECTION_ESTABLISHED)
    {
        m_SendCounters.insert(connection_id, 0);

        // Set system 3d settings
        if (const auto error =
            com::teamspeak::pluginsdk::funcs::sound::systemset_3d_settings(connection_id, 1.0f, 0.1f);
            error)
            Error("(on_connect_status_changed)", connection_id, error);
        else
            Log("System 3D Settings set.");
    }
    else if (newStatus == STATUS_DISCONNECTED)
    {
        m_universe.delete_items(connection_id);
        m_PlayersInMyContext.remove(connection_id);
        m_SendCounters.remove(connection_id);
    }
}

void PositionalAudio::set_my_vr(std::wstring_view value)
{
    meObj.set_vr(value);
    if (value.empty())
        emit BroadcastJSON(QStringLiteral("{\"me\":true}"));

    emit myVrChanged(QString::fromStdWString(meObj.get_vr()));
}

void PositionalAudio::set_my_identity(std::wstring_view value)
{
    meObj.setIdentityRaw(value);
    emit myIdentityChanged(QString::fromStdWString(meObj.getIdentity()));  // potentionally different from raw
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

}  // namespace thorwe
