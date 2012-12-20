#ifndef TSFUNCTIONS_H
#define TSFUNCTIONS_H

#include "public_errors.h"
#include "public_errors_rare.h"
#include "public_definitions.h"
#include "public_rare_definitions.h"
#include "ts3_functions.h"

#include <QObject>
#include <QtCore>
#include <QMutex>

#include "ts_settings_qt.h"

// Ptt Enum
enum PTT_CHANGE_STATUS {
    PTT_ACTIVATE = 0,
    PTT_DEACTIVATE,
    PTT_TOGGLE
};

class TSFunctions : public QObject
{
    Q_OBJECT

public:
    static TSFunctions* instance() {
        static QMutex mutex;
        if(!m_Instance) {
            mutex.lock();

            if(!m_Instance)
                m_Instance = new TSFunctions;

            mutex.unlock();
        }
        return m_Instance;
    }

    static void drop() {
        static QMutex mutex;
        mutex.lock();
        delete m_Instance;
        m_Instance = 0;
        mutex.unlock();
    }

    // Error, Debug etc.
    bool GetErrorSound(QString &in);
    bool GetInfoIcon(QString &in);
    void PlayErrorSound(uint64 serverConnectionHandlerID);
    void PlayErrorSound();
    void Error(uint64 scHandlerID, unsigned int error, char *message);
    void Print(QString message, LogLevel logLevel);
    void Print(QString message);
    void Log(QString message, LogLevel logLevel);
    void Log(QString message);

    // Getters
    uint64 GetActiveServerConnectionHandlerID();

    // Server interaction
    int SetActiveServer(uint64 serverConnectionHandlerID);
    int SetActiveServerRelative(uint64 serverConnectionHandlerID, bool next);
    inline int SetNextActiveServer(uint64 serverConnectionHandlerID) { return SetActiveServerRelative(serverConnectionHandlerID, true); }
    inline int SetPrevActiveServer(uint64 serverConnectionHandlerID) { return SetActiveServerRelative(serverConnectionHandlerID, false); }

    unsigned int GetActiveServerRelative(uint64 serverConnectionHandlerID, bool next, uint64 *result);

    // Communication
    int SetPushToTalk(uint64 serverConnectionHandlerID, PTT_CHANGE_STATUS action);
    int SetPushToTalk(uint64 scHandlerID, bool shouldTalk);

    /* Playback */
    unsigned int GetPlaybackConfigValueAsFloat(uint64 serverConnectionHandlerID, const char *ident, float *result);
    unsigned int SetPlaybackConfigValue(uint64 serverConnectionHandlerID, const char *ident, const char *value);

    //Mixed
    unsigned int GetClientId(uint64 serverConnectionHandlerID, anyID *result);
    unsigned int GetChannelClientList(uint64 serverConnectionHandlerID, uint64 channelID, anyID **result);

    // How I like it
    unsigned int GetClientUID(uint64 serverConnectionHandlerID, anyID clientID, QString &result);
    unsigned int GetSubChannels(uint64 serverConnectionHandlerID, uint64 channelId, QList<uint64> *result);
    unsigned int SetWhisperList(uint64 serverConnectionHandlerID, GroupWhisperType groupWhisperType, GroupWhisperTargetMode groupWhisperTargetMode);

    /* Push-to-talk */
    bool pttActive;
    bool vadActive;
    bool inputActive;

    void Init();

    TSSettings Settings;

    void onConnectStatusChangeEvent(uint64 serverConnectionHandlerID, int newStatus, unsigned int errorNumber);

    void ParseCommand(char *cmd, char *arg);

    QString GetConfigPath();
    QString GetFullConfigPath();

    QString GetLanguage();
    bool InitLocalization();
    void ShutdownLocalization();
    QTranslator* translator;

    uint64 GetServerHandler(QString name, uint64 *result);
signals:
    void Command(char *cmd, char *arg);
    void PttDelayFinished();

public slots:
    void onPttDelayFinished();

private:
    explicit TSFunctions();
    ~TSFunctions();
    static TSFunctions* m_Instance;
    TSFunctions(const TSFunctions &);
    TSFunctions& operator=(const TSFunctions &);

    static const int PATH_BUFSIZE = 512; //ToDo: Maybe move that to Constructor with value passing of plugin.cpp define

    void UpdatePttDelayInfo();
    bool pttDelayEnabled;
    int pttDelayMsec;

    QTimer* timer;
    QMutex* mutex;
    QMutex* command_mutex;

    QMap<uint64,anyID> myClientIDs;


};

#endif // TSFUNCTIONS_H
