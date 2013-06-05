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

    // forwarded from plugin.cpp
    void onConnectStatusChangeEvent(uint64 serverConnectionHandlerID, int newStatus, unsigned int errorNumber);

    void onServerGroupListEvent(uint64 serverConnectionHandlerID, uint64 serverGroupID, const char* name, int type, int iconID, int saveDB);
    void onServerGroupListFinishedEvent(uint64 serverConnectionHandlerID);

signals:
    void connectStatusChanged(uint64 serverConnectionHandlerID, int newStatus, unsigned int errorNumber);

    void serverGroupListUpdated(uint64 serverConnectionHandlerID, QMap<uint64,QString>);

public slots:

private slots:

private:
    explicit TSServersInfo();
    ~TSServersInfo();
    static TSServersInfo* m_Instance;
    TSServersInfo(const TSServersInfo &);
    TSServersInfo& operator=(const TSServersInfo &);

    TSServerInfo* _GetServerInfo(uint64 serverConnectionHandlerID, bool createOnNotExist);
    QMap<uint64,QPointer<TSServerInfo> > m_serverInfoMap;
};

#endif // TS_SERVERSINFO_H
