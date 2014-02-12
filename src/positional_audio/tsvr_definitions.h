#ifndef TSVR_DEFINITIONS_H
#define TSVR_DEFINITIONS_H

#include <QString>
#include <QTextStream>

class CustomEnvironmentSupportInterface
{
public:
    virtual bool onIdentityRawDirty(QString rawIdentity) = 0;
    virtual QString getIdentity() const = 0;
    virtual bool onInfoData(QTextStream &data) = 0;
};
Q_DECLARE_INTERFACE(CustomEnvironmentSupportInterface,"net.thorwe.CrossTalk.CustomEnvironmentSupportInterface/1.0")

#endif // TSVR_DEFINITIONS_H
