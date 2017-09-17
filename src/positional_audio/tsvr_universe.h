#pragma once

#include <QtCore/QObject>
#include <QtCore/QMap>

#include "tsvr_obj_other.h"

class TsVrUniverse : public QObject
{
    Q_OBJECT
public:
    explicit TsVrUniverse(QObject *parent = 0);
    
    TsVrObjOther* Add(uint64 serverConnectionHandlerID, anyID clientID);
    void Remove(uint64 serverConnectionHandlerID, anyID clientID);
    void Remove(uint64 serverConnectionHandlerID);
    void Remove();
    bool Contains(uint64 serverConnectionHandlerID, anyID clientID);
    TsVrObjOther* Get(uint64 serverConnectionHandlerID, anyID clientID);

//    bool RegisterCustomEnvironmentSupport(QObject *p);
//    QMap<QString,QObject*> m_CustomEnvironmentSupportMap;

signals:
    void removed(QString);

public slots:
    void onConnectStatusChanged(uint64 serverConnectionHandlerID, int newStatus, unsigned int errorNumber);

private:
    QMap<uint64,QMap<anyID,TsVrObjOther*>* > m_Map;
};
