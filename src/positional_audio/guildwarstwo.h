#ifndef GUILDWARSTWO_H
#define GUILDWARSTWO_H

#include <QObject>

class GuildWarsTwo : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString myIdentity
               READ getMyIdentity
               NOTIFY myIdentityChanged)
    Q_PROPERTY(quint32 professionId
               READ getMyProfessionId
               NOTIFY myProfessionIdChanged)
    Q_PROPERTY(quint32 teamColorId
               READ getMyTeamColorId
               NOTIFY myTeamColorIdChanged)
    Q_PROPERTY(bool commander
               READ isMeCommander
               NOTIFY myCommanderStatusChanged)

public:
    explicit GuildWarsTwo(QObject *parent = 0);

    bool onIdentityRawDirty(QString rawIdentity);
    bool onContextRawDirty(QByteArray rawContext);

    QString getMyIdentity() const;
    quint32 getMyProfessionId() const;
    quint32 getMyTeamColorId() const;
    bool isMeCommander() const;

signals:
    void myIdentityChanged(QString);
    void myProfessionIdChanged(quint32);
    void myTeamColorIdChanged(quint32);
    void myCommanderStatusChanged(bool);

public slots:


private:
    QString m_Identity;
    quint32 m_professionId;
    quint32 m_teamColorId;
    bool m_isCommander;
};

#endif // GUILDWARSTWO_H
