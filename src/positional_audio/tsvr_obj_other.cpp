#include "tsvr_obj_other.h"

#include "teamspeak/public_errors.h"
#include "core/ts_helpers_qt.h"
#include "core/ts_logging_qt.h"

TsVrObjOther::TsVrObjOther(QObject *parent, uint64 serverConnectionHandlerID, anyID clientID) :
    m_serverConnectionHandlerID(serverConnectionHandlerID),
    m_clientID(clientID)
{
    this->setParent(parent);
    resetAvatar();

    unsigned int error;
    if ((error = TSHelpers::GetClientUID(serverConnectionHandlerID,clientID,m_clientUID)) != ERROR_ok)
        TSLogging::Error("(TsVrObjOther::TsVrObjOther)",serverConnectionHandlerID,error,true);
}

uint64 TsVrObjOther::getServerConnectionHandlerID() const
{
    return m_serverConnectionHandlerID;
}

anyID TsVrObjOther::getClientID() const
{
    return m_clientID;
}

bool TsVrObjOther::setAvatar(TS3_VECTOR position, TS3_VECTOR front, TS3_VECTOR top)
{
    bool isDirtyPosition = !(m_Avatar_Pos == position);
    if (isDirtyPosition)
    {
        m_Avatar_Pos.x = position.x;
        m_Avatar_Pos.y = position.y;
        m_Avatar_Pos.z = position.z;
    }
    bool isDirtyFront = !(m_Avatar_Front == front);
    if (isDirtyFront)
    {
        m_Avatar_Front.x = front.x;
        m_Avatar_Front.y = front.y;
        m_Avatar_Front.z = front.z;
    }
    bool isDirtyTop = !(m_Avatar_Top == top);
    if (isDirtyTop)
    {
        m_Avatar_Top.x = top.x;
        m_Avatar_Top.y = top.y;
        m_Avatar_Top.z = top.z;
    }
    bool isDirty = (isDirtyPosition || isDirtyFront || isDirtyTop);
    if (isDirty)
        emit avatarChanged(this,isDirtyPosition,isDirtyFront,isDirtyTop);

    return isDirty;
}
