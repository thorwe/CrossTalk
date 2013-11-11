#ifndef MOD_POSITIONALAUDIO_H
#define MOD_POSITIONALAUDIO_H

#include <QObject>
#include <QtCore>
#include "../module.h"
#include "tsvr_universe.h"
#include "tsvr_obj_self.h"
#include "../ts_infodata_qt.h"
//#include "../ts_context_menu_qt.h"
#include "definitions_positionalaudio.h"

#ifndef RETURNCODE_BUFSIZE
#define RETURNCODE_BUFSIZE 128
#endif

class PositionalAudio : public Module, public InfoDataInterface//, public ContextMenuInterface
{
    Q_OBJECT
    Q_INTERFACES(InfoDataInterface)// ContextMenuInterface)
    Q_PROPERTY(QString myVr
               READ getMyVr
               NOTIFY myVrChanged)
    Q_PROPERTY(QString myIdentity
               READ getMyIdentity
               NOTIFY myIdentityChanged)
    Q_PROPERTY(bool useCamera
               READ isUseCamera
               WRITE setUseCamera
               NOTIFY useCameraChanged)

public:
    explicit PositionalAudio(QObject *parent = 0);

    // events forwarded from plugin.cpp
    bool onPluginCommand(uint64 serverConnectionHandlerID, anyID clientID, bool isMe, QString cmd, QTextStream &args);
    void onClientMoveEvent(uint64 serverConnectionHandlerID, anyID clientID, uint64 oldChannelID, uint64 newChannelID, int visibility, anyID myID);
    void onCustom3dRolloffCalculationClientEvent(uint64 serverConnectionHandlerID, anyID clientID, float distance, float* volume);

    //Properties
    QString getMyVr() const;
    QString getMyIdentity() const;

    bool onInfoDataChanged(uint64 serverConnectionHandlerID, uint64 id, enum PluginItemType type, uint64 mine, QTextStream &data);

//    int onServerErrorEvent(uint64 serverConnectionHandlerID, const char* errorMessage, unsigned int error, const char* returnCode, const char* extraMessage);

    bool isUseCamera() const;

    QMap<QString,PositionalAudio_ServerSettings> getServerSettings() const;

    //bool RegisterCustomEnvironmentSupport(QObject *p);
    TsVrObjSelf* meObj;

signals:
    void myVrChanged(QString);
    void myIdentityChanged(QString);
    void useCameraChanged(bool val);
    void serverBlock(QString);

public slots:
    void onConnectStatusChanged(uint64 serverConnectionHandlerID, int newStatus, unsigned int errorNumber);
    void setUseCamera(bool val);

    void AddServerSetting(QString serverUniqueId, QString serverName);
    void RemoveServerSetting(QString serverUniqueId, QString serverName);
    void setServerSettingEnabled(QString serverUniqueId, bool val);
    void setServerSettingBlocked(QString serverUniqueId, bool val);
    void setServerSettingSendInterval(QString serverUniqueId, float val);
    void setServerSettingSendIntervalSilentInc(QString serverUniqueId, float val);

    void onMyVrChanged(TsVrObj* obj, QString val);
    void onMyIdentityChanged(TsVrObj* obj, QString val);

protected:
    void onRunningStateChanged(bool value);
    void timerEvent(QTimerEvent *event);

private:
    void unlock();
    bool trylock();
    bool fetch();

    int m_tryTimerId;

    int m_fetchTimerId;
    static const int FETCH_TIMER_INTERVAL = 20; //Mumble fetches these values 50 times a second aka 20msec
    static const int SEND_THROTTLE_GLOBAL = 5;  // Send(): 50times/sec; count to 5 -> 10times/sec; save some computation
    static const int SEND_INTERVAL_MODIFIER = 50 / SEND_THROTTLE_GLOBAL;    // I...think.
    int m_fetchTimerElapsed;
    int m_fetchTimerTimeoutMsec;

    // myself
    //QString m_Identity;
    QString m_IdentityUncleaned;
    bool m_isDirty_IdentityUncleaned;
    QByteArray m_Context;
    //QString m_ContextHex;
    bool m_Context_Dirty;
    bool m_Avatar_Dirty;

    ulong m_lastCount;

    QMultiMap<uint64,anyID> m_PlayersInMyContext;

    TsVrUniverse* universe;

    void Update3DListenerAttributes();

    bool m_isUseCamera;

    QString GetSendString(bool isAll);
    QString GetSendStringJson(bool isAll, bool isMe, TsVrObj *obj);
    void Send(uint64 serverConnectionHandlerID, QString args, int targetMode, const anyID *targetIDs, const char *returnCode);
    void Send();
    void Send(QString args, int targetMode);

    bool m_IsSendAllOverride;
    int m_sendCounter;
    int m_silentSendCounter;

    TS3_VECTOR NULL_VECTOR;

    QSharedMemory* m_sharedMemory;

    QMap<QString,PositionalAudio_ServerSettings> m_ServerSettings;
    QHash<uint64,int> m_SendCounters;
};
QTextStream &operator<<(QTextStream &out, const TS3_VECTOR &ts3Vector);
QTextStream &operator>>(QTextStream &in, TS3_VECTOR &ts3Vector);
//bool operator==(const TS3_VECTOR &vec, const float *arr);

#endif // MOD_POSITIONALAUDIO_H
