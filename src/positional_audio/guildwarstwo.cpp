#include "guildwarstwo.h"

#include <QStringList>
#include "ts_helpers_qt.h"
#include "gw2_world_names.h"
#include "gw2_map_names.h"
#include "tsvr_obj.h"

#include "ts_logging_qt.h"

#if QT_VERSION >= 0x050000
  const QUrl GW2_BUILD("https://api.guildwars2.com/v1/build.json");
  const QUrl GW2_WORLD_NAMES("https://api.guildwars2.com/v1/world_names.json");
  const QUrl GW2_CONTINENTS("https://api.guildwars2.com/v1/continents.json");
  const QUrl GW2_MAPS("https://api.guildwars2.com/v1/maps.json");
#endif

GuildWarsTwo::GuildWarsTwo(QObject *parent) :
    QObject(parent)
{
    this->setObjectName("Guild Wars 2");
//    PositionalAudio* pa = qobject_cast<PositionalAudio *>(parent);
//    pa->RegisterCustomEnvironmentSupport(this);

#if QT_VERSION >= 0x050000
    m_netwManager = new QNetworkAccessManager(this);
    connect(m_netwManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(onNetwManagerFinished(QNetworkReply*)));
    QNetworkRequest request(GW2_BUILD);
    m_netwManager->get(request);
#endif
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

#if QT_VERSION >= 0x050000
void GuildWarsTwo::onNetwManagerFinished(QNetworkReply *reply)
{
    if (reply->error() != QNetworkReply::NoError)
    {
        TSLogging::Log(reply->errorString(),LogLevel_WARNING);
        return;
    }

    QJsonParseError jsonError;
    QJsonDocument doc = QJsonDocument::fromJson(reply->readAll(),&jsonError);
    if (jsonError.error != QJsonParseError::ParseError::NoError)
    {
        TSLogging::Error(QString("%1: Json error: %2").arg(this->objectName()).arg(jsonError.errorString()),true);
        return;
    }
    if ((reply->url()) == GW2_BUILD)
    {
        if (!doc.isObject())
        {
            TSLogging::Error(QString("%1: Build is not an object.").arg(this->objectName()),true);
            return;
        }
        m_build_id = doc.object().value("build_id").toInt();
        TSLogging::Log(QString("%1: Build Id: %2").arg(this->objectName()).arg(m_build_id),LogLevel_INFO);
    }

    // path
    bool isPathOk = true;
    QString path = TSHelpers::GetConfigPath();
    QDir dir(path);
    if (!dir.exists())
    {
        TSLogging::Error(QString("%1: Config Path does not exist").arg(this->objectName()));
        isPathOk = false;
    }
    else
    {
        path = QString(ts3plugin_name()).toLower();
        if (!dir.exists(path))
        {
            //TSLogging::Print("Path does not exist: " + path);
            if (!dir.mkdir(path))
            {
                TSLogging::Error(QString("%1: Could not create cache folder.").arg(this->objectName()),true);
                isPathOk = false;
            }
        }
        if (isPathOk && !dir.cd(path))
        {
            TSLogging::Error(QString("%1: Could not enter cache folder.").arg(this->objectName()));
            isPathOk = false;
        }
    }
    // /path

    if (isPathOk)
    {
        QString filename = reply->url().fileName();
        //TSLogging::Print("filename: " + filename);

        bool isFileExists = dir.exists(filename);

        QFile file(this);
        file.setFileName(dir.absoluteFilePath(filename));
        if (!file.open(QIODevice::ReadWrite))
            TSLogging::Error(QString("%1: Could not open file: %2").arg(this->objectName()).arg(file.fileName()));
        else
        {
            if ((reply->url()) == GW2_BUILD)
            {
                if (isFileExists)
                {
                    QByteArray saveData = file.readAll();
                    QJsonDocument cache_doc = QJsonDocument::fromJson(saveData,&jsonError);
                    if (jsonError.error != QJsonParseError::ParseError::NoError)
                    {
                        TSLogging::Error(QString("%1: Json error: %2").arg(this->objectName()).arg(jsonError.errorString()),true);
                        return;
                    }
                    if (!cache_doc.isObject())
                    {
                        TSLogging::Error(QString("%1: Build is not an object.").arg(this->objectName()),true);
                        return;
                    }
                    quint32 cache_build_id = cache_doc.object().value("build_id").toInt();
                    if (cache_build_id != m_build_id)
                    {
                        TSLogging::Log(QString("%1: Build Id: changed.").arg(this->objectName()), LogLevel_INFO);
                        QNetworkRequest requestWorldNames(GW2_WORLD_NAMES);
                        m_netwManager->get(requestWorldNames);
                        QNetworkRequest requestContinents(GW2_CONTINENTS);
                        m_netwManager->get(requestContinents);
                        QNetworkRequest requestMaps(GW2_MAPS);
                        m_netwManager->get(requestMaps);
                    }
                    else
                        TSLogging::Log(QString("%1: Build Id: up to date.").arg(this->objectName()), LogLevel_INFO);
                }
                else
                {
                    TSLogging::Log(QString("%1: Build Id: no cached value.").arg(this->objectName()), LogLevel_INFO);
                    QNetworkRequest requestWorldNames(GW2_WORLD_NAMES);
                    m_netwManager->get(requestWorldNames);
                    QNetworkRequest requestContinents(GW2_CONTINENTS);
                    m_netwManager->get(requestContinents);
                    QNetworkRequest requestMaps(GW2_MAPS);
                    m_netwManager->get(requestMaps);
                }
            }
            else
            {
                // ToDo: Sane reformating of JSON data
                if ((reply->url()) == GW2_CONTINENTS)
                {
                    QJsonObject obj = doc.object();
                    obj = obj.value("continents").toObject();
                    doc.setObject(obj);
                    m_Continents = obj;
                }
                else if ((reply->url()) == GW2_MAPS)
                {
                    QJsonObject obj = doc.object();
                    obj = obj.value("maps").toObject();
                    doc.setObject(obj);
                    m_Maps = obj;
                }
                else if ((reply->url()) == GW2_WORLD_NAMES)
                {
                    QJsonArray arr = doc.array();
                    QJsonObject obj;
                    for (int i = 0; i<arr.size(); ++i)
                    {
                        QJsonObject val = arr.at(i).toObject();
                        obj.insert(val.value("id").toString(), val);
                    }
                    doc.setObject(obj);
                    m_WorldNames = obj;
                }
            }
            file.write(doc.toJson());
        }
        file.close();

    }
    reply->deleteLater();
}
#endif

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
