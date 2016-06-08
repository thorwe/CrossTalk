#pragma once

#include <QObject>
#include "tsvr_obj.h"

class TsVrObjOther : public TsVrObj
{
    Q_OBJECT
    Q_PROPERTY(uint64 serverConnectionHandlerID
               READ getServerConnectionHandlerID)
    Q_PROPERTY(anyID clientID
               READ getClientID)
public:
    explicit TsVrObjOther(QObject *parent = 0, uint64 serverConnectionHandlerID = 0, anyID clientID = 0);

    uint64 getServerConnectionHandlerID() const;
    anyID getClientID() const;
    QString getClientUID() const {return m_clientUID;}

    bool setAvatar(TS3_VECTOR position, TS3_VECTOR front, TS3_VECTOR top);

signals:
    
public slots:

private:
    uint64 m_serverConnectionHandlerID;
    anyID m_clientID;
    QString m_clientUID;
};
