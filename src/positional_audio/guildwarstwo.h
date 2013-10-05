#ifndef GUILDWARSTWO_H
#define GUILDWARSTWO_H

#include <QObject>
#include "tsvr_definitions.h"

class GuildWarsTwo : public QObject, public CustomEnvironmentSupportInterface
{
    Q_OBJECT
    Q_INTERFACES(CustomEnvironmentSupportInterface)
    Q_PROPERTY(QString identity
               READ getIdentity
               NOTIFY identityChanged)
    Q_PROPERTY(quint32 professionId
               READ getProfessionId
               NOTIFY professionIdChanged)
    Q_PROPERTY(quint8 mapId
               READ getMapId
               NOTIFY mapIdChanged)
    Q_PROPERTY(quint32 worldId
               READ getWorldId
               NOTIFY worldIdChanged)
    Q_PROPERTY(quint32 teamColorId
               READ getTeamColorId
               NOTIFY teamColorIdChanged)
    Q_PROPERTY(bool commander
               READ isCommander
               NOTIFY commanderStatusChanged)

public:
    explicit GuildWarsTwo(QObject *parent = 0);

    bool onIdentityRawDirty(QString rawIdentity);
//    bool onContextRawDirty(QByteArray rawContext);

    QString getIdentity() const;
    quint32 getProfessionId() const;
    quint8 getMapId() const;
    quint32 getWorldId() const;
    quint32 getTeamColorId() const;
    bool isCommander() const;

signals:
    void identityChanged(QString);
    void professionIdChanged(quint32);
    void mapIdChanged(quint8);
    void worldIdChanged(quint32);
    void teamColorIdChanged(quint32);
    void commanderStatusChanged(bool);


public slots:


private:
    QString m_Identity;
    quint32 m_professionId;
    quint8 m_mapId;
    quint32 m_worldId;
    quint32 m_teamColorId;
    bool m_isCommander;
};

#endif // GUILDWARSTWO_H
