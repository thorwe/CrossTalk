#ifndef MOD_POSITIONALAUDIO_H
#define MOD_POSITIONALAUDIO_H

#include <QObject>
#include <QtCore>
#include "../module.h"
#include "universe.h"
#include "../ts_infodata_qt.h"
//#include "../ts_context_menu_qt.h"
#include "definitions_positionalaudio.h"

#ifndef RETURNCODE_BUFSIZE
#define RETURNCODE_BUFSIZE 128
#endif

class CustomEnvironmentSupportInterface
{
public:
    virtual QString onIdentityRawDirty(QString rawIdentity) = 0;
};
Q_DECLARE_INTERFACE(CustomEnvironmentSupportInterface,"net.thorwe.CrossTalk.CustomEnvironmentSupportInterface/1.0")

class PositionalAudio : public Module, public InfoDataInterface//, public ContextMenuInterface
{
    Q_OBJECT
    Q_INTERFACES(InfoDataInterface)// ContextMenuInterface)
    Q_PROPERTY(QString myGame
               READ getMyGame
               NOTIFY myGameChanged)
    Q_PROPERTY(QString myGameDescription
               READ getMyGameDescription
               NOTIFY myGameDescriptionChanged)
    Q_PROPERTY(QString myIdentity
               READ getMyIdentity
               NOTIFY myIdentityChanged)
    Q_PROPERTY(bool useCamera
               READ isUseCamera
               WRITE setUseCamera
               NOTIFY useCameraChanged)
    Q_PROPERTY(QByteArray myContext
               READ getMyContext)
public:
    explicit PositionalAudio(QObject *parent = 0);

    // events forwarded from plugin.cpp
    bool onPluginCommand(uint64 serverConnectionHandlerID, anyID clientID, bool isMe, QString cmd, QTextStream &args);
    void onClientMoveEvent(uint64 serverConnectionHandlerID, anyID clientID, uint64 oldChannelID, uint64 newChannelID, int visibility, anyID myID);
    void onCustom3dRolloffCalculationClientEvent(uint64 serverConnectionHandlerID, anyID clientID, float distance, float* volume);

    //Properties
    QString getMyGame() const;
    QString getMyGameDescription() const;
    QString getMyIdentity() const;
    QByteArray getMyContext() const;

    bool onInfoDataChanged(uint64 serverConnectionHandlerID, uint64 id, enum PluginItemType type, uint64 mine, QTextStream &data);

//    int onServerErrorEvent(uint64 serverConnectionHandlerID, const char* errorMessage, unsigned int error, const char* returnCode, const char* extraMessage);

    bool isUseCamera() const;

    QMap<QString,PositionalAudio_ServerSettings> getServerSettings() const;

    bool RegisterCustomEnvironmentSupport(QObject *p);

signals:
    void myGameChanged(QString);
    void myGameDescriptionChanged(QString);
    void myIdentityChanged(QString);

    void gameChanged(uint64 serverConnectionHandlerID, anyID clientID, QString gameName);
    void identityChanged(uint64 serverConnectionHandlerID, anyID clientID, QString gameName);

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

protected:
    void onRunningStateChanged(bool value);
    void timerEvent(QTimerEvent *event);

private:
    void unlock();
    bool trylock();
    bool fetch();

    inline void UpdateMyGame();
    QString m_GameName;
    QString m_GameDesc;

    int m_tryTimerId;

    int m_fetchTimerId;
    static const int FETCH_TIMER_INTERVAL = 20; //Mumble fetches these values 50 times a second aka 20msec
    static const int SEND_THROTTLE_GLOBAL = 5;  // Send(): 50times/sec; count to 5 -> 10times/sec; save some computation
    static const int SEND_INTERVAL_MODIFIER = 50 / SEND_THROTTLE_GLOBAL;    // I...think.
    int m_fetchTimerElapsed;
    int m_fetchTimerTimeoutMsec;

    // myself
    QString m_Identity;
    QString m_IdentityUncleaned;
    bool m_Identity_Dirty;
    QByteArray m_Context;
    QString m_ContextHex;
    bool m_Context_Dirty;
    TS3_VECTOR m_Avatar_Pos;
    TS3_VECTOR m_Avatar_Front;
    TS3_VECTOR m_Avatar_Top;
    bool m_Avatar_Dirty;
    TS3_VECTOR m_Camera_Pos;
    TS3_VECTOR m_Camera_Front;
    TS3_VECTOR m_Camera_Top;

    ulong m_lastCount;

    QMultiMap<uint64,anyID> m_PlayersInMyContext;

    Universe* universe;

    void Update3DListenerAttributes();

    bool m_isUseCamera;

    QString GetSendString(bool isAll);
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

    QMap<QString,QObject*> m_CustomEnvironmentSupportMap;
    QObject* m_CustomEnvironmentSupport;
};
QTextStream &operator<<(QTextStream &out, const TS3_VECTOR &ts3Vector);
QTextStream &operator>>(QTextStream &in, TS3_VECTOR &ts3Vector);
bool operator==(const TS3_VECTOR &vec, const float *arr);

#endif // MOD_POSITIONALAUDIO_H
