#include "guildwarstwo.h"

#include <QStringList>
#include "ts_logging_qt.h"

GuildWarsTwo::GuildWarsTwo(QObject *parent) :
    QObject(parent)
{
    this->setObjectName("Guild Wars 2");
}

QString GuildWarsTwo::getMyIdentity() const
{
    return m_Identity;
}

quint32 GuildWarsTwo::getMyProfessionId() const
{
    return m_professionId;
}

quint32 GuildWarsTwo::getMyTeamColorId() const
{
    return m_teamColorId;
}

bool GuildWarsTwo::isMeCommander() const
{
    return m_isCommander;
}

bool GuildWarsTwo::onIdentityRawDirty(QString rawIdentity)
{
    bool isNeedsUpdate = false;

    QStringList stringList = rawIdentity.split(",",QString::KeepEmptyParts,Qt::CaseSensitive);

    QString string = stringList[0];
    string.remove(0,10);
    string.remove(string.length() - 1 , 1);
    if (m_Identity != string)
    {
        m_Identity = string;
        isNeedsUpdate = true;
        emit myIdentityChanged(m_Identity);
    }

    //profession
    string = stringList[1];
    bool ok;
    unsigned int profession = string.split(": ",QString::KeepEmptyParts,Qt::CaseSensitive).at(1).toUInt(&ok,10);
    if (ok)
    {
        if (m_professionId != profession)
        {
            m_professionId = profession;
            isNeedsUpdate = true;
            emit myProfessionIdChanged(m_professionId);
        }
    }
    else
        TSLogging::Error("Could not determine profession.");

    //map id
//    string = stringList[2];
//    unsigned int map_id = string.split(": ",QString::KeepEmptyParts,Qt::CaseSensitive).at(1).toUInt(&ok,10);
//    if (ok)
//        ts3Functions.printMessageToCurrentTab(QString("Map Id: %1").arg(map_id).toLocal8Bit().constData());
//    else
//        Error("Could not determine map id.");

    //world id
//    string = stringList[3];
//    unsigned long world_id = string.split(": ",QString::KeepEmptyParts,Qt::CaseSensitive).at(1).toULong(&ok,10);
//    if (ok)
//        ts3Functions.printMessageToCurrentTab(QString("World Id: %1").arg(world_id).toLocal8Bit().constData());
//    else
//    {
//        Error("Could not determine world id.");
//        ts3Functions.printMessageToCurrentTab(string.split(": ",QString::KeepEmptyParts,Qt::CaseSensitive).at(1).toLocal8Bit().constData());
//    }

    //team color id
    string = stringList[4];
    unsigned int team_color_id = string.split(": ",QString::KeepEmptyParts,Qt::CaseSensitive).at(1).toUInt(&ok,10);
    if (ok)
    {
        if (m_teamColorId != team_color_id)
        {
            m_teamColorId = team_color_id;
            isNeedsUpdate = true;
            emit myTeamColorIdChanged(m_teamColorId);
        }
    }
    else
        TSLogging::Error("Could not determine team color id.");

    //commander
    string = stringList[5];
    bool isCommander = (string.split(": ",QString::KeepEmptyParts,Qt::CaseSensitive).at(1) == "true");
    if (isCommander != m_isCommander)
    {
        m_isCommander = isCommander;
        isNeedsUpdate = true;
        emit myCommanderStatusChanged(m_isCommander);
    }

    return isNeedsUpdate;
}

bool GuildWarsTwo::onContextRawDirty(QByteArray rawContext)
{
    return false;   // TODO
}
