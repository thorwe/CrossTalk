#include "minecraft.h"

Minecraft::Minecraft(QObject *parent) :
    QObject(parent)
{
    this->setObjectName("Minecraft");
}

QString Minecraft::getIdentity() const
{
    if (m_meObj.value("name") != QJsonValue::Undefined)
        return m_meObj.value("name").toString(QString::null);

    return QString::null;
}

bool Minecraft::onIdentityRawDirty(QString rawIdentity)
{
    QJsonObject oldObj = m_meObj;

    if (rawIdentity.isEmpty())
    {

        m_meObj.remove("name");
        m_meObj["worldSpawn"] = { 0, 0, 0 };
        m_meObj["dimension"] = 0;

        emit identityChanged(QString::null);

        return true;
    }

    QJsonParseError jsonError;
    QJsonDocument meDoc = QJsonDocument::fromJson(rawIdentity.toUtf8(),&jsonError);
    if (jsonError.error != QJsonParseError::ParseError::NoError)
    {
        TSLogging::Error(QString("%1: Json error: %2").arg(this->objectName()).arg(jsonError.errorString()),true);
        return false;
    }
    if (!meDoc.isObject())
    {
        TSLogging::Error(QString("%1: QJsonDocument is not an object.").arg(this->objectName()),true);
        return false;
    }
    m_meObj = meDoc.object();

    bool isIdentityDirty = false;
    if (oldObj.value("name") != m_meObj.value("name"))
    {
        isIdentityDirty = true;
        emit identityChanged(m_meObj.value("name").toString(QString::null));
    }

    return isIdentityDirty;
}

bool Minecraft::onInfoData(QTextStream &data)
{
    data << "\n";

    QJsonArray worldSpawn = m_meObj.value("worldSpawn").toArray();
    if (!worldSpawn.isEmpty())
    {
        data << worldSpawn.at(0) << ", " << worldSpawn.at(1) << ", " << worldSpawn.at(2) << "\n";
    }
    data << "Dimension: " << m_meObj.value("dimension").toInt();

    data << " )\n";

    return true;
}


