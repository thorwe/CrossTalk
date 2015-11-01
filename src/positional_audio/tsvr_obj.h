#ifndef TSVR_OBJ_H
#define TSVR_OBJ_H

#include <QObject>
#include <QTextStream>
#include "teamspeak/public_definitions.h"

#include "tsvr_definitions.h"

class TsVrObj : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString vr
               READ getVr
               WRITE setVr)
    Q_PROPERTY(QString vrDescription
               READ getVrDescription
               WRITE setVrDescription)
    Q_PROPERTY(QString context
               READ getContext
               WRITE setContext)
    Q_PROPERTY(QString identity
               READ getIdentity)
    Q_PROPERTY(QString identityRaw
               READ getIdentityRaw
               WRITE setIdentityRaw)

public:
    explicit TsVrObj(QObject *parent = 0);

    QString getVr() const;
    QString getVrDescription() const;
    QString getContext() const;
    QString getIdentity() const;
    QString getIdentityRaw() const;
    TS3_VECTOR getAvatarPosition() const;
    TS3_VECTOR getAvatarFront() const;
    TS3_VECTOR getAvatarTop() const;

    bool onInfoDataChanged(QTextStream &data);

    void setCustomEnvironmentSupport(QObject* val);

signals:
    void vrChanged(TsVrObj*,QString);
    void vrDescriptionChanged(TsVrObj*,QString);
    void contextChanged(TsVrObj*,QString);
    void identityChanged(TsVrObj*,QString);
    void identityRawChanged(TsVrObj*,QString);
    void avatarChanged(TsVrObj*,bool,bool,bool);

public slots:
    void setVr(QString val);
    void setVrDescription(QString val);
    void setContext(QString val);
    void setIdentityRaw(QString val);
    void resetAvatar();

protected:
    TS3_VECTOR m_Avatar_Pos;
    TS3_VECTOR m_Avatar_Front;
    TS3_VECTOR m_Avatar_Top;

private:
    QString m_vr;
    QString m_vr_desc;
    QString m_context;
    QString m_identityRaw;

    QObject* m_CustomEnvironmentSupport;
};

bool operator==(const TS3_VECTOR &veca, const TS3_VECTOR &vecb);
bool operator==(const TS3_VECTOR &vec, const float *arr);
#endif // TSVR_OBJ_H
