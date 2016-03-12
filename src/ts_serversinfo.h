#ifndef TS_SERVERSINFO_H
#define TS_SERVERSINFO_H

#include "ts_serverinfo_qt.h"

#include <QObject>

class TSServersInfo : public QObject
{
    Q_OBJECT

public:
    static TSServersInfo* instance()
    {
        static QMutex mutex;
        if(!m_Instance)
        {
            mutex.lock();

            if(!m_Instance)
                m_Instance = new TSServersInfo;

            mutex.unlock();
        }
        return m_Instance;
    }

    static void drop()
    {
        static QMutex mutex;
        mutex.lock();
        delete m_Instance;
        m_Instance = 0;
        mutex.unlock();
    }
    
    TSServerInfo* GetServerInfo(uint64 serverConnectionHandlerID);
    uint64 FindServerByUniqueId(QString server_id);

    // forwarded from plugin.cpp
    void onConnectStatusChangeEvent(uint64 serverConnectionHandlerID, int newStatus, unsigned int errorNumber);

    void onServerGroupListEvent(uint64 serverConnectionHandlerID, uint64 serverGroupID, const char* name, int type, int iconID, int saveDB);
    void onServerGroupListFinishedEvent(uint64 serverConnectionHandlerID);
    void onChannelGroupListEvent(uint64 serverConnectionHandlerID, uint64 channelGroupID, const char* name, int type, int iconID, int saveDB);
    void onChannelGroupListFinishedEvent(uint64 serverConnectionHandlerID);

signals:
    void connectStatusChanged(uint64 serverConnectionHandlerID, int newStatus, unsigned int errorNumber);

    void serverGroupListUpdated(uint64 serverConnectionHandlerID, QMap<uint64,QString>);

public slots:

private slots:

private:
    TSServersInfo() = default;
    ~TSServersInfo() = default;
    static TSServersInfo* m_Instance;
    TSServersInfo(const TSServersInfo &);
    TSServersInfo& operator=(const TSServersInfo &);

    TSServerInfo* _GetServerInfo(uint64 serverConnectionHandlerID, bool createOnNotExist = false);
    QMap<uint64,QPointer<TSServerInfo> > m_serverInfoMap;
};

#endif // TS_SERVERSINFO_H
