#pragma once

#include "plugin_qt.h"
#include "shared_mem_reader.h"
#include "tsvr_obj_self.h"
#include "tsvr_universe.h"

#include "core/definitions.h"
#include "core/module_qt.h"

#include "definitions_positionalaudio.h"

#include "gsl/pointers"

#include <QtCore/QObject>
#include <QtCore/QtCore>

#include <string>

using namespace com::teamspeak;

namespace thorwe
{

class PositionalAudio : public Module_Qt, public InfoDataInterface
{
    Q_OBJECT
    Q_INTERFACES(InfoDataInterface)

  public:
    PositionalAudio(Plugin& plugin);

    // events forwarded from plugin.cpp
    bool onPluginCommand(
    connection_id_t connection_id, client_id_t clientID, bool isMe, QString cmd, QTextStream &args);
    void onClientMoveEvent(connection_id_t connection_id,
                           client_id_t clientID,
                           uint64 oldChannelID,
                           uint64 newChannelID,
                           int visibility,
                           client_id_t myID);
    void onCustom3dRolloffCalculationClientEvent(connection_id_t connection_id,
                                                 client_id_t clientID,
                                                 float distance,
                                                 float *volume);

    //Properties
    std::wstring get_my_vr() const;
    std::wstring getMyIdentity() const;

    bool onInfoDataChanged(
    connection_id_t connection_id, uint64 id, enum PluginItemType type, uint64 mine, QTextStream &data);

    //    int onServerErrorEvent(connection_id_t connection_id, const char* errorMessage, unsigned
    //    int error, const char* returnCode, const char* extraMessage);

    bool isUseCamera() const;
    bool isUseAttenuation() const;
    int getDistanceMin() const;
    int getDistanceMax() const;
    float getRollOff() const;
    float getRollOffMax() const;

    bool isPositioned(client_id_t clientID) const;

    QMap<QString,PositionalAudio_ServerSettings> getServerSettings() const;

    TsVrObjSelf meObj{};

    void on_connect_status_changed(connection_id_t connection_id, int new_status, unsigned int error_number);

  signals:
    void myVrChanged(QString);
    void myIdentityChanged(QString);
    void useCameraChanged(bool);
    void useAttenuationChanged(bool);
    void distanceMinChanged(int);
    void distanceMaxChanged(int);
    void rollOffChanged(float);
    void rollOffMaxChanged(float);
    void serverBlock(QString);

    void BroadcastJSON(QString);

  public slots:
    void setUseCamera(bool val);
    void setUseAttenuation(bool val);
    void setDistanceMin(int val);
    void setDistanceMax(int val);
    void setRollOff(float val);
    void setRollOffMax(float val);

    void AddServerSetting(QString serverUniqueId, QString serverName);
    void RemoveServerSetting(QString serverUniqueId, QString serverName);
    void setServerSettingEnabled(QString serverUniqueId, bool val);
    void setServerSettingBlocked(QString serverUniqueId, bool val);
    void setServerSettingSendInterval(QString serverUniqueId, float val);
    void setServerSettingSendIntervalSilentInc(QString serverUniqueId, float val);

    void onMyIdentityChanged(TsVrObj *obj, QString val);

  protected:
    void onRunningStateChanged(bool value);
    void timerEvent(QTimerEvent *event);

private:
    void unlock();
    bool trylock();
    bool fetch();

    int m_tryTimerId = 0;

    int m_fetchTimerId = 0;

    int m_fetchTimerElapsed = 0;
    int m_fetchTimerTimeoutMsec = 5000;

    void set_my_vr(std::wstring_view = L"");
    void set_my_identity(std::wstring_view = L"");
    void remove_other(connection_id_t, client_id_t);

    // myself
    bool m_isDirty_IdentityUncleaned = false;
    bool m_Context_Dirty = false;
    bool m_Avatar_Dirty = false;

    ulong m_lastCount = 0;

    QMultiMap<connection_id_t, client_id_t> m_PlayersInMyContext;

    thorwe::TsVrUniverse m_universe;

    void Update3DListenerAttributes();

    bool m_isUseCamera = true;
    bool m_isUseAttenuation = false;
    int m_distanceMin = 0;
    int m_distanceMax = 0;
    float m_rollOff = 0.0f;
    float m_rollOffMax = 0.0f;
    //float m_rollOff_Lin;
    float m_rollOffMax_Lin = 1.0f;

    QString GetSendString(bool isAll);
    QString GetSendStringJson(bool isAll, bool isMe, TsVrObj *obj);
    void Send(connection_id_t connection_id,
              const QString &args,
              int targetMode,
              const std::vector<client_id_t> &target_ids);
    void Send();
    void Send(const QString &args, int targetMode);

    bool m_IsSendAllOverride = true;
    int m_sendCounter = 0;
    int m_silentSendCounter = 2;

    TS3_VECTOR NULL_VECTOR{0.f, 0.f, 0.f};

    gsl::owner<QSharedMemory *> m_shared_memory = nullptr;

    QMap<QString, PositionalAudio_ServerSettings> m_ServerSettings;
    QHash<connection_id_t, int> m_SendCounters;

    Talkers& m_talkers;
    TSInfoData& m_info_data;
    TSServersInfo& m_servers_info;
    ServerThreaded &m_websockets_server;

    thorwe::Shared_Mem_Reader m_shared_mem_reader;
};
QTextStream &operator<<(QTextStream &out, const TS3_VECTOR &ts3Vector);
QTextStream &operator>>(QTextStream &in, TS3_VECTOR &ts3Vector);
//bool operator==(const TS3_VECTOR &vec, const float *arr);

}  // namespace thorwe
