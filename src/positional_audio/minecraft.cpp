#include "minecraft.h"
#include "core/ts_logging_qt.h"
#include <QtCore/QJsonArray>
#include <QtCore/QJsonDocument>

Minecraft::Minecraft(QObject *parent)
    : QObject(parent)
{
    this->setObjectName("Minecraft");
}

auto Minecraft::get_identity() const -> std::wstring
{
    if (m_meObj.value("name") != QJsonValue::Undefined)
        return m_meObj.value("name").toString(QString::null).toStdWString();

    return {};
}

auto Minecraft::on_identity_raw_dirty(std::wstring_view raw_identity) -> bool
{
    QJsonObject oldObj = m_meObj;

    if (raw_identity.empty())
    {

        m_meObj.remove("name");
        m_meObj["worldSpawn"] = QJsonArray{0, 0, 0};
        m_meObj["dimension"] = 0;

        emit identityChanged(QString::null);

        return true;
    }

    const auto raw_w = std::wstring(raw_identity.data(), raw_identity.size());
    QJsonParseError jsonError{};
    QJsonDocument meDoc = QJsonDocument::fromJson(QString::fromStdWString(raw_w).toUtf8(), &jsonError);
    if (jsonError.error != QJsonParseError::ParseError::NoError)
    {
        TSLogging::Error(QString("%1: Json error: %2").arg(this->objectName(), jsonError.errorString()),
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
        emit identityChanged(m_meObj.value("name").toString(QString::null));
    }

    return isIdentityDirty;
}

auto Minecraft::onInfoData(QTextStream &data) -> bool
{
    data << "\n";

    QJsonArray worldSpawn = m_meObj.value("worldSpawn").toArray();
    /*if (!worldSpawn.isEmpty())
    {
        data << worldSpawn.at(0) << ", " << worldSpawn.at(1) << ", " << worldSpawn.at(2) << "\n";
    }*/
    data << "Dimension: " << m_meObj.value("dimension").toInt();

    data << " )\n";

    return true;
}
