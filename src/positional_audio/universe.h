#pragma once

#include <QtCore/QObject>
#include <QtCore/QMap>
#include "teamspeak/public_definitions.h"

struct TsVR {
    QString vr;
    QString context;
    QString identity;
    TS3_VECTOR avatarPosition;
    TS3_VECTOR avatarFront;
    TS3_VECTOR avatarTop;
};

class Universe : public QObject
{
    Q_OBJECT
public:
    explicit Universe(QObject *parent = 0);

    TsVR* Add(uint64 serverConnectionHandlerID, anyID clientID);
    void Remove(uint64 serverConnectionHandlerID, anyID clientID);
    void Remove(uint64 serverConnectionHandlerID);
    void Remove();
    bool Contains(uint64 serverConnectionHandlerID, anyID clientID);
    TsVR* Get(uint64 serverConnectionHandlerID, anyID clientID);

signals:
    
public slots:
    void onConnectStatusChanged(uint64 serverConnectionHandlerID, int newStatus, unsigned int errorNumber);

private:
    QMap<uint64,QMap<anyID,TsVR*>* > UniverseMap;
};
