#include "guildwarstwo.h"

#include <QStringList>
#include "ts_helpers_qt.h"
#include "gw2_world_names.h"
#include "gw2_map_names.h"
#include "tsvr_obj.h"

#include "ts_logging_qt.h"

GuildWarsTwo::GuildWarsTwo(QObject *parent) :
    QObject(parent)
{
    this->setObjectName("Guild Wars 2");
//    PositionalAudio* pa = qobject_cast<PositionalAudio *>(parent);
//    pa->RegisterCustomEnvironmentSupport(this);
}

QString GuildWarsTwo::getIdentity() const
{
    return m_Identity;
}

quint32 GuildWarsTwo::getProfessionId() const
{
    return m_professionId;
}

quint8 GuildWarsTwo::getMapId() const
{
    return m_mapId;
}

quint32 GuildWarsTwo::getWorldId() const
{
    return m_worldId;
}

quint32 GuildWarsTwo::getTeamColorId() const
{
    return m_teamColorId;
}

bool GuildWarsTwo::isCommander() const
{
    return m_isCommander;
}

bool GuildWarsTwo::onIdentityRawDirty(QString rawIdentity)
{
    if (rawIdentity.isEmpty())
    {
        bool isIdentityDirty = (!m_Identity.isEmpty());
        m_Identity = QString::null;
        emit identityChanged(m_Identity);

        if (m_professionId != 0)
        {
            m_professionId = 0;
            emit professionIdChanged(m_professionId);
        }

        if (m_mapId != 0)
        {
            m_mapId = 0;
            emit mapIdChanged(m_mapId);
        }
        if (m_worldId != 0)
        {
            m_worldId = 0;
            emit worldIdChanged(m_worldId);
        }
        if (m_teamColorId != 0)
        {
            m_teamColorId = 0;
            emit teamColorIdChanged(m_teamColorId);
        }
        if (m_isCommander)
        {
            m_isCommander = false;
            emit commanderStatusChanged(m_isCommander);
        }
        return isIdentityDirty;
    }

    QStringList stringList = rawIdentity.split(",",QString::KeepEmptyParts,Qt::CaseSensitive);

    QString string = stringList[0];
    string.remove(0,10);
    string.remove(string.length() - 1 , 1);
    bool isIdentityDirty = (m_Identity != string);
    if (isIdentityDirty)
    {
        m_Identity = string;
//        TSLogging::Print(QString("Idendity changed: %1").arg(m_Identity));
        emit identityChanged(m_Identity);
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
//            TSLogging::Print(QString("Profession changed: %1").arg(m_professionId));
            emit professionIdChanged(m_professionId);
        }
    }
    else
        TSLogging::Error("Could not determine profession.");

    //map id
    string = stringList[2];
    unsigned int map_id = string.split(": ",QString::KeepEmptyParts,Qt::CaseSensitive).at(1).toUInt(&ok,10);
    if (ok)
    {
        if (m_mapId != map_id)
        {
            m_mapId = map_id;
//            TSLogging::Print(QString("Map Id changed: %1").arg(m_mapId));
            emit mapIdChanged(m_mapId);
        }
    }
    else
        TSLogging::Error("Could not determine map id.");

    //world id
    string = stringList[3];
    unsigned long world_id = string.split(": ",QString::KeepEmptyParts,Qt::CaseSensitive).at(1).toULong(&ok,10);
    if (ok)
    {
        if (m_worldId != world_id)
        {
            if (world_id >= 1001 && world_id <= 2301)
            {
                m_worldId = world_id;
    //            TSLogging::Print(QString("World Id changed: %1").arg(m_worldId));
                emit worldIdChanged(m_worldId);
            }
        }
    }
    else
    {
        TSLogging::Error("Could not determine world id.");
    }

    //team color id
    string = stringList[4];
    unsigned int team_color_id = string.split(": ",QString::KeepEmptyParts,Qt::CaseSensitive).at(1).toUInt(&ok,10);
    if (ok)
    {
        if (m_teamColorId != team_color_id)
        {
            m_teamColorId = team_color_id;
//            TSLogging::Print(QString("Team color Id changed: %1").arg(team_color_id));
            emit teamColorIdChanged(m_teamColorId);
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
//        TSLogging::Print(QString("isCommander changed: %1").arg(m_isCommander));
        emit commanderStatusChanged(m_isCommander);
    }

    return isIdentityDirty;
}

//! Add additional information for a specific game
bool GuildWarsTwo::onInfoData(QTextStream &data)
{
    data << "\n";
    data << "is commander: " << ((isCommander())?"y":"n") << "\n";

    QString lang = TSHelpers::GetLanguage();

    data << GW2::getMapName(getMapId(),lang) << " ( ";
    if (m_teamColorId != 0) // WvW
    {
        data << "WvW Team: " << getTeamColorId();
    }
    else                    // Tyria
    {
        data << GW2::getWorldName(getWorldId(),lang);
    }
    data << " )\n";


    // get the position from the game independent parent object
    //TsVrObj* obj = (TsVrObj*)this->parent();
    //TS3_VECTOR pos = obj->getAvatarPosition();
    //data << "Pos: " << pos.x << "," << pos.y << "," << pos.z;

    return true;
}

// as long as this is the only occurence where a game puts known readable data in context and this data is available already,
// let's for now stick to getting map and world id from the name string
//bool GuildWarsTwo::onContextRawDirty(QByteArray rawContext)
//{
//    return false;   // TODO
//}
