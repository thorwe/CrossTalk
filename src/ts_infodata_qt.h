#pragma once

#include <QObject>
#include <QtCore>
//#include <QMutex>

#include "teamspeak/public_definitions.h"
#include "plugin_definitions.h"

class InfoDataInterface
{
public:
    virtual bool onInfoDataChanged(uint64 serverConnectionHandlerID, uint64 id, enum PluginItemType type, uint64 mine, QTextStream &data) = 0;
};
Q_DECLARE_INTERFACE(InfoDataInterface,"net.thorwe.CrossTalk.InfoDataInterface/1.0")

class TSInfoData : public QObject
{
    Q_OBJECT
    Q_PROPERTY(PluginItemType infoType
               READ getInfoType
               NOTIFY infoTypeChanged)
    Q_PROPERTY(uint64 infoId
               READ getInfoId
               NOTIFY infoIdChanged)
public:
    static TSInfoData* instance() {
        static QMutex mutex;
        if(!m_Instance) {
            mutex.lock();

            if(!m_Instance)
                m_Instance = new TSInfoData;

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

    PluginItemType getInfoType() const;
    uint64 getInfoId() const;

//    void setHomeId(uint64 serverConnectionHandlerID);
    
    bool Register(QObject *p, bool isRegister, int priority);
    void onInfoData(uint64 serverConnectionHandlerID, uint64 id, PluginItemType type, char **data);

signals:
    void infoTypeChanged(PluginItemType);
    void infoIdChanged(uint64);
    
public slots:
    void RequestUpdate(uint64 serverConnectionHandlerID, uint64 id, PluginItemType type);
    void RequestUpdate(uint64 serverConnectionHandlerID, uint64 id) {RequestUpdate(serverConnectionHandlerID,id,PLUGIN_CHANNEL);}
    void RequestUpdate(uint64 serverConnectionHandlerID, anyID id) {RequestUpdate(serverConnectionHandlerID,id,PLUGIN_CLIENT);}
    void RequestUpdate(uint64 serverConnectionHandlerID) {RequestUpdate(serverConnectionHandlerID,NULL,PLUGIN_SERVER);}
    void RequestSelfUpdate();

private:
    //singleton
    explicit TSInfoData();
    ~TSInfoData() = default;
    static TSInfoData* m_Instance;
    TSInfoData(const TSInfoData &);
    TSInfoData& operator=(const TSInfoData &);

    PluginItemType m_InfoType = PLUGIN_SERVER;
    uint64 m_InfoId = 0;

    uint64 m_homeId = 0;

    QList<QPointer<QObject> > m_Callbacks;
};
