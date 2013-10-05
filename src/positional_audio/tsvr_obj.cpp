#include "tsvr_obj.h"

#include "guildwarstwo.h"

TsVrObj::TsVrObj(QObject *parent) :
    QObject(parent),
    m_CustomEnvironmentSupport(NULL)
{
}

// Getters

QString TsVrObj::getVr() const
{
    return m_vr;
}

QString TsVrObj::getVrDescription() const
{
    return m_vr_desc;
}

QString TsVrObj::getContext() const
{
    return m_context;
}

QString TsVrObj::getIdentity() const
{
    if (m_CustomEnvironmentSupport != NULL)
    {
        CustomEnvironmentSupportInterface *iCustomEnvironmentSupport = qobject_cast<CustomEnvironmentSupportInterface *>(m_CustomEnvironmentSupport);
        return iCustomEnvironmentSupport->getIdentity();
    }
    else
        return m_identityRaw;
}

QString TsVrObj::getIdentityRaw() const
{
    return m_identityRaw;
}

TS3_VECTOR TsVrObj::getAvatarPosition() const
{
    return m_Avatar_Pos;
}

TS3_VECTOR TsVrObj::getAvatarFront() const
{
    return m_Avatar_Front;
}

TS3_VECTOR TsVrObj::getAvatarTop() const
{
    return m_Avatar_Top;
}

// Slot Setters

void TsVrObj::setCustomEnvironmentSupport(QObject *val)
{
    m_CustomEnvironmentSupport = val;
}

void TsVrObj::setVr(QString val)
{
    if (m_vr == val)
        return;

    m_vr = val;

    if (m_CustomEnvironmentSupport != NULL)
        m_CustomEnvironmentSupport->deleteLater();

    if (m_vr.isEmpty())
        m_CustomEnvironmentSupport = NULL;
    else if (m_vr == "Guild Wars 2")
        m_CustomEnvironmentSupport = new GuildWarsTwo(this);

    emit vrChanged(this, m_vr);
}

void TsVrObj::setVrDescription(QString val)
{
    if (m_vr_desc == val)
        return;

    m_vr_desc = val;
    emit vrDescriptionChanged(this,m_vr_desc);
}

void TsVrObj::setContext(QString val)
{
    if (m_context == val)
        return;

    m_context = val;
    emit contextChanged(this,m_context);
}

void TsVrObj::setIdentityRaw(QString val)
{
    if (m_identityRaw == val)
        return;

    m_identityRaw = val;
    emit identityRawChanged(this,m_identityRaw);

    if (m_CustomEnvironmentSupport != NULL)
    {
        CustomEnvironmentSupportInterface *iCustomEnvironmentSupport = qobject_cast<CustomEnvironmentSupportInterface *>(m_CustomEnvironmentSupport);
        if (iCustomEnvironmentSupport->onIdentityRawDirty(m_identityRaw))
            emit identityChanged(this,iCustomEnvironmentSupport->getIdentity());
    }
}

void TsVrObj::resetAvatar()
{
    m_Avatar_Pos.x = 0.0f;
    m_Avatar_Pos.y = 0.0f;
    m_Avatar_Pos.z = 0.0f;

    m_Avatar_Front.x = 0.0f;
    m_Avatar_Front.y = 0.0f;
    m_Avatar_Front.z = 0.0f;

    m_Avatar_Top.x = 0.0f;
    m_Avatar_Top.y = 0.0f;
    m_Avatar_Top.z = 0.0f;
}

bool operator==(const TS3_VECTOR &veca, const TS3_VECTOR &vecb)
{
    return ((veca.x == vecb.x) && (veca.y == vecb.y) && (veca.z == vecb.z));
}

bool operator==(const TS3_VECTOR &vec, const float *arr)
{
    return ((vec.x == arr[0]) && (vec.y == arr[1]) && (vec.z == arr[2]));
}
