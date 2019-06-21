#pragma once

#include <QtCore/QObject>
#include <QtNetwork/QtNetwork>
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
    bool onInfoData(QTextStream &data);

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

private slots:
    void onNetwManagerFinished(QNetworkReply *reply);
    void onSslErrors(QNetworkReply *reply, const QList<QSslError> &errors);

private:
    QNetworkAccessManager *m_netwManager;

    QJsonObject m_meObj;

    // GW2 API
    quint32 m_build_id;
    QJsonObject m_Continents;
    QJsonObject m_WorldNames;
    QJsonObject m_Maps;
};
