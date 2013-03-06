#ifndef TS_INFODATA_QT_H
#define TS_INFODATA_QT_H

#include <QObject>
#include <QtCore>
//#include <QMutex>

#include "public_definitions.h"
#include "plugin_definitions.h"

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

    void setHomeId(uint64 serverConnectionHandlerID);
    
signals:
    void infoTypeChanged(PluginItemType);
    void infoIdChanged(uint64);
    
public slots:
    void requestUpdate(uint64 serverConnectionHandlerID);
    void requestUpdate(uint64 serverConnectionHandlerID, uint64 channelID);
    void requestUpdate(uint64 serverConnectionHandlerID, anyID clientID);

    void SetInfo(uint64 serverConnectionHandlerID, uint64 id, PluginItemType type, QString ModuleName, QString info);

private:
    //singleton
    explicit TSInfoData();
    ~TSInfoData();
    static TSInfoData* m_Instance;
    TSInfoData(const TSInfoData &);
    TSInfoData& operator=(const TSInfoData &);

    PluginItemType m_InfoType;
    uint64 m_InfoId;

    uint64 m_homeId;

    QMap<QPair<uint64,QString>,QString> m_Map_PLUGIN_SERVER;
    QMultiMap<QPair<uint64,uint64>, QPair<QString,QString> > m_Map_PLUGIN_CHANNEL;
    QMultiMap<QPair<uint64,anyID>, QPair<QString,QString> > m_Map_PLUGIN_CLIENT;
};

#endif // TS_INFODATA_QT_H
