#pragma once

#include <QtCore/QObject>
#include <QtCore/QJsonObject>
#include "tsvr_definitions.h"

class Minecraft : public QObject, public CustomEnvironmentSupportInterface
{
    Q_OBJECT
    Q_INTERFACES(CustomEnvironmentSupportInterface)

  public:
    explicit Minecraft(QObject *parent = 0);

    std::wstring get_identity() const;

    bool on_identity_raw_dirty(std::wstring_view raw_identity);
    bool onInfoData(QTextStream &data);

  signals:
    void identityChanged(QString);

private:
    QJsonObject m_meObj;

};
