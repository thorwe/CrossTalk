#include "version_qt.h"

#include <QList>
#include <QStringList>
#include "ts_logging_qt.h"

VersionQt::VersionQt(QObject *parent, QString versionStr) :
    QObject(parent),
    m_versionString(versionStr)
{
    if (m_versionString.isEmpty())
    {
        m_versionString = "0.0.0.0";
        return;
    }

    QList<int> versionIntList;
    QStringList versionStringList = versionStr.split(".",QString::SkipEmptyParts);

    for (int i = 0; i < versionStringList.length(); ++i)
    {
        bool isOk;
        int versionInt = versionStringList[i].toInt(&isOk,10);
        if (!isOk)
        {
            versionIntList.insert(i, 0);
            m_isValid = false;
        }
        else
        {
            versionIntList.insert(i,versionInt);
        }
    }
    versionStringList.clear();

    int versionIntListSize = versionIntList.size();

    if (versionIntListSize > 0)
        m_major = versionIntList.at(0);

    if (versionIntListSize > 1)
        m_minor = versionIntList.at(1);

    if (versionIntListSize > 2)
        m_revision = versionIntList.at(2);

    if (versionIntListSize > 3)
        m_build = versionIntList.at(3);

    if (m_major < 0) m_major = 0;
    if (m_minor < 0) m_minor = 0;
    if (m_revision < 0) m_revision = 0;
    if (m_build < 0) m_build = 0;
}

int VersionQt::major() const
{
    return m_major;
}

int VersionQt::minor() const
{
    return m_minor;
}

int VersionQt::revision() const
{
    return m_revision;
}

int VersionQt::build() const
{
    return m_build;
}

bool VersionQt::valid() const
{
    return m_isValid;
}

QString VersionQt::toString() const
{
    return m_versionString;
    //return QString("%1.%2.%3.%4").arg(m_major).arg(m_minor).arg(m_revision).arg(m_build);
}

bool VersionQt::operator<(const VersionQt &other) const
{
    int o_val = other.major();
    if (m_major != o_val)
        return (m_major < o_val);

    o_val = other.minor();
    if (m_minor != o_val)
        return (m_minor < o_val);

    o_val = other.revision();
    if (m_revision != o_val)
        return (m_revision < o_val);

    o_val = other.build();
    if (m_build != o_val)
        return (m_build < o_val);

    return false;
}

bool VersionQt::operator==(const VersionQt &other) const
{
    return m_major == (other.major())
        && m_minor == (other.minor())
        && m_revision == (other.revision())
        && m_build == (other.build());
}

