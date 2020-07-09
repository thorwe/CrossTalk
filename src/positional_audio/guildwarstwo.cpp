#include "guildwarstwo.h"

#include "core/ts_helpers_qt.h"
#include "tsvr_obj.h"

#include "core/ts_logging_qt.h"

const QUrl GW2_BUILD("https://api.guildwars2.com/v1/build.json");
const QUrl GW2_WORLD_NAMES("https://api.guildwars2.com/v2/worlds?ids=all");
const QUrl GW2_CONTINENTS("https://api.guildwars2.com/v1/continents.json");
const QUrl GW2_MAPS("https://api.guildwars2.com/v1/maps.json");


GuildWarsTwo::GuildWarsTwo(QObject *parent)
    : QObject(parent)
{
    this->setObjectName("Guild Wars 2");
    //    PositionalAudio* pa = qobject_cast<PositionalAudio *>(parent);
    //    pa->RegisterCustomEnvironmentSupport(this);

    m_netwManager = new QNetworkAccessManager(this);
    connect(m_netwManager, &QNetworkAccessManager::finished, this, &GuildWarsTwo::onNetwManagerFinished);
    connect(m_netwManager, &QNetworkAccessManager::sslErrors, this, &GuildWarsTwo::onSslErrors);
    QNetworkRequest request(GW2_BUILD);
    m_netwManager->get(request);
}

std::wstring GuildWarsTwo::get_identity() const
{
    if (m_meObj.value("name") != QJsonValue::Undefined)
        return m_meObj.value("name").toString(QString::null).toStdWString();

    return {};
}

quint32 GuildWarsTwo::getProfessionId() const
{
    return m_meObj.value("profession").toInt();
}

quint8 GuildWarsTwo::getMapId() const
{
    return m_meObj.value("map_id").toInt();
}

quint32 GuildWarsTwo::getWorldId() const
{
    return m_meObj.value("world_id").toInt();
}

quint32 GuildWarsTwo::getTeamColorId() const
{
    return m_meObj.value("team_color_id").toInt();
}

bool GuildWarsTwo::isCommander() const
{
    return m_meObj.value("commander").toBool();
}

void GuildWarsTwo::onNetwManagerFinished(QNetworkReply *reply)
{
    if (reply->error() != QNetworkReply::NoError)
    {
        TSLogging::Log(reply->errorString(), LogLevel_WARNING);
        return;
    }

    auto jsonError = QJsonParseError{};
    QJsonDocument doc = QJsonDocument::fromJson(reply->readAll(), &jsonError);
    if (jsonError.error != QJsonParseError::ParseError::NoError)
    {
        TSLogging::Error(QString("%1: Json error: %2").arg(this->objectName()).arg(jsonError.errorString()),
                         true);
        return;
    }
    if ((reply->url()) == GW2_BUILD)
    {
        if (!doc.isObject())
        {
            TSLogging::Error(QString("%1: Build is not an object.").arg(this->objectName()), true);
            return;
        }
        m_build_id = doc.object().value("build_id").toInt();
        TSLogging::Log(QString("%1: Build Id: %2").arg(this->objectName()).arg(m_build_id), LogLevel_INFO);
    }

    // path
    QDir dir;
    bool isPathOk = TSHelpers::GetCreatePluginConfigFolder(dir);

    if (isPathOk)
    {
        QString filename = reply->url().fileName();
        // TSLogging::Log("filename: " + filename,LogLevel_DEBUG);

        bool isFileExists = dir.exists(filename);

        QFile file(this);
        file.setFileName(dir.absoluteFilePath(filename));
        if (!file.open(QIODevice::ReadWrite))
            TSLogging::Error(
            QString("%1: Could not open file: %2").arg(this->objectName()).arg(file.fileName()));
        else
        {
            if ((reply->url()) == GW2_BUILD)
            {
                if (isFileExists)
                {
                    QByteArray saveData = file.readAll();
                    QJsonDocument cache_doc = QJsonDocument::fromJson(saveData, &jsonError);
                    if (jsonError.error != QJsonParseError::ParseError::NoError)
                    {
                        TSLogging::Error(
                        QString("%1: Json error: %2").arg(this->objectName()).arg(jsonError.errorString()),
                        true);
                        return;
                    }
                    if (!cache_doc.isObject())
                    {
                        TSLogging::Error(QString("%1: Build is not an object.").arg(this->objectName()),
                                         true);
                        return;
                    }
                    quint32 cache_build_id = cache_doc.object().value("build_id").toInt();
                    if (cache_build_id != m_build_id)
                    {
                        TSLogging::Log(QString("%1: Build Id: changed.").arg(this->objectName()),
                                       LogLevel_INFO);
                        QNetworkRequest requestWorldNames(GW2_WORLD_NAMES);
                        m_netwManager->get(requestWorldNames);
                        QNetworkRequest requestContinents(GW2_CONTINENTS);
                        m_netwManager->get(requestContinents);
                        QNetworkRequest requestMaps(GW2_MAPS);
                        m_netwManager->get(requestMaps);
                    }
                    else
                        TSLogging::Log(QString("%1: Build Id: up to date.").arg(this->objectName()),
                                       LogLevel_INFO);
                }
                else
                {
                    TSLogging::Log(QString("%1: Build Id: no cached value.").arg(this->objectName()),
                                   LogLevel_INFO);
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
                if (reply->url().hasQuery())
                {
                    /*QUrlQuery query(reply->url());
                    if (query.hasQueryItem("lang"))
                        TSLogging::Log(QString("Lang Query:
                    ").arg(query.queryItemValue("lang")),LogLevel_DEBUG);*/

                    return;
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
                        for (int i = 0; i < arr.size(); ++i)
                        {
                            QJsonObject val = arr.at(i).toObject();
                            obj.insert(val.value("id").toString(), val);
                        }
                        doc.setObject(obj);
                        m_WorldNames = obj;
                    }

                    // lang
                    /*QUrl url = reply->url();
                    url.setQuery("lang=de");
                    QNetworkRequest requestDe(url);
                    m_netwManager->get(requestDe);
                    url.setQuery("lang=fr");
                    QNetworkRequest requestFr(url);
                    m_netwManager->get(requestFr);
                    url.setQuery("lang=es");
                    QNetworkRequest requestEs(url);
                    m_netwManager->get(requestEs);*/
                }
            }
            file.resize(0);
            file.write(doc.toJson());
        }
        file.close();
    }
    reply->deleteLater();
}

void GuildWarsTwo::onSslErrors(QNetworkReply *reply, const QList<QSslError> &errors)
{
    Q_UNUSED(reply);
    foreach (const QSslError &error, errors)
    {
        TSLogging::Error(QString("%1 SSL Error: %2").arg(this->objectName()).arg(error.errorString()), true);
    }
}

bool GuildWarsTwo::on_identity_raw_dirty(std::wstring_view raw_identity)
{
    QJsonObject oldObj = m_meObj;

    if (raw_identity.empty())
    {
        m_meObj.remove("name");
        m_meObj["profession"] = 0;
        m_meObj["map_id"] = 0;
        m_meObj["world_id"] = 0;
        m_meObj["team_color_id"] = 0;
        m_meObj["commander"] = false;

        /*emit identityChanged(QString::null);
        emit professionIdChanged(0);
        emit mapIdChanged(0);
        emit worldIdChanged(0);

        if (oldObj.value("team_color_id").toInt() != 0)
            emit teamColorIdChanged(0);
        if (oldObj.value("commander").toBool() == true)
            emit commanderStatusChanged(false);*/

        return true;
    }
    const auto raw_q = std::wstring(raw_identity);
    QJsonParseError jsonError;
    QJsonDocument meDoc = QJsonDocument::fromJson(QString::fromStdWString(raw_q).toUtf8(), &jsonError);
    if (jsonError.error != QJsonParseError::ParseError::NoError)
    {
        TSLogging::Error(QString("%1: Json error: %2").arg(this->objectName()).arg(jsonError.errorString()),
                         true);
        return false;
    }
    if (!meDoc.isObject())
    {
        TSLogging::Error(QString("%1: QJsonDocument is not an object.").arg(this->objectName()), true);
        return false;
    }
    m_meObj = meDoc.object();

    bool isIdentityDirty = false;
    if (oldObj.value("name") != m_meObj.value("name"))
    {
        isIdentityDirty = true;
        // emit identityChanged(m_meObj.value("name").toString(QString::null));
    }
    /*if (oldObj.value("profession") != m_meObj.value("profession"))
        emit professionIdChanged(m_meObj.value("profession").toInt());
    if (oldObj.value("map_id") != m_meObj.value("map_id"))
        emit mapIdChanged(m_meObj.value("map_id").toInt());
    if (oldObj.value("team_color_id") != m_meObj.value("team_color_id"))
        emit teamColorIdChanged(m_meObj.value("team_color_id").toInt());
    if (oldObj.value("commander") != m_meObj.value("commander"))
        emit commanderStatusChanged(m_meObj.value("commander").toBool());*/

    if (oldObj.value("world_id") != m_meObj.value("world_id"))
    {
        quint32 newWorldId = m_meObj.value("world_id").toInt();
        if (newWorldId >= 1001 && newWorldId <= 2301)
        {
            // emit mapIdChanged(m_meObj.value("world_id").toInt());
        }
        else
        {
            quint32 oldWorldId = oldObj.value("world_id").toInt();
            if (oldWorldId >= 1001 && oldWorldId <= 2301)
                m_meObj["world_id"] = oldObj.value("world_id");
        }
    }

    return isIdentityDirty;
}

//! Add additional information for a specific game
bool GuildWarsTwo::onInfoData(QTextStream &data)
{
    data << "\n";
    data << "is commander: " << ((isCommander()) ? "y" : "n") << "\n";

    data << m_Maps.value(QString::number(getMapId())).toObject().value("map_name").toString();

    quint32 team_color_id = getTeamColorId();
    if (team_color_id != 0)  // WvW
    {
        data << "WvW Team: " << team_color_id;
    }
    else  // Tyria
    {
        data << m_WorldNames.value(QString::number(getWorldId())).toObject().value("name").toString();
    }

    data << " )\n";


    // get the position from the game independent parent object
    // TsVrObj* obj = (TsVrObj*)this->parent();
    // TS3_VECTOR pos = obj->getAvatarPosition();
    // data << "Pos: " << pos.x << "," << pos.y << "," << pos.z;

    return true;
}
