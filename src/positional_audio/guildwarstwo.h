#pragma once

#include "tsvr_definitions.h"

#include "gsl/pointers"

#include <QtCore/QObject>
#include <QtNetwork/QtNetwork>

#include <cstdint>

class GuildWarsTwo : public QObject, public CustomEnvironmentSupportInterface
{
    Q_OBJECT
    Q_INTERFACES(CustomEnvironmentSupportInterface)

public:
    explicit GuildWarsTwo(QObject *parent = 0);

    bool on_identity_raw_dirty(std::wstring_view raw_identity);
    bool onInfoData(QTextStream &data);

    std::wstring get_identity() const;
    quint32 getProfessionId() const;
    quint8 getMapId() const;
    quint32 getWorldId() const;
    quint32 getTeamColorId() const;
    bool isCommander() const;

private slots:
    void onNetwManagerFinished(QNetworkReply *reply);
    void onSslErrors(QNetworkReply *reply, const QList<QSslError> &errors);

  private:
    gsl::owner<QNetworkAccessManager *> m_netwManager;

    QJsonObject m_meObj;

    // GW2 API
    uint32_t m_build_id{0};
    QJsonObject m_Continents;
    QJsonObject m_WorldNames;
    QJsonObject m_Maps;
};
