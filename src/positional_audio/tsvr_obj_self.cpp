#include "tsvr_obj_self.h"

TsVrObjSelf::TsVrObjSelf(QObject *parent)
{
    this->setParent(parent);
    resetAvatar();
    resetCamera();
}

TS3_VECTOR TsVrObjSelf::getCameraPosition() const
{
    return m_Camera_Pos;
}

TS3_VECTOR TsVrObjSelf::getCameraFront() const
{
    return m_Camera_Front;
}

TS3_VECTOR TsVrObjSelf::getCameraTop() const
{
    return m_Camera_Top;
}

bool TsVrObjSelf::setAvatar(const float *position, const float *front, const float *top)
{
    bool isDirtyPosition = !(m_Avatar_Pos == position);
    if (isDirtyPosition)
    {
        m_Avatar_Pos.x = position[0];
        m_Avatar_Pos.y = position[1];
        m_Avatar_Pos.z = position[2];
    }

    bool isDirtyFront = !(m_Avatar_Front == front);
    if (isDirtyFront)
    {
        m_Avatar_Front.x = front[0];
        m_Avatar_Front.y = front[1];
        m_Avatar_Front.z = front[2];
    }

    bool isDirtyTop = !(m_Avatar_Top == top);
    if (isDirtyTop)
    {
        m_Avatar_Top.x = top[0];
        m_Avatar_Top.y = top[1];
        m_Avatar_Top.z = top[2];
    }

    bool isDirty = (isDirtyPosition || isDirtyFront || isDirtyTop);
    if (isDirty)
        emit avatarChanged(this,isDirtyPosition,isDirtyFront,isDirtyTop);

    return isDirty;
}

bool TsVrObjSelf::setCamera(const float *position, const float *front, const float *top)
{
    bool isDirtyPosition = !(m_Camera_Pos == position);
    if (isDirtyPosition)
    {
        m_Camera_Pos.x = position[0];
        m_Camera_Pos.y = position[1];
        m_Camera_Pos.z = position[2];
    }

    bool isDirtyFront = !(m_Camera_Front == front);
    if (isDirtyFront)
    {
        m_Camera_Front.x = front[0];
        m_Camera_Front.y = front[1];
        m_Camera_Front.z = front[2];
    }

    bool isDirtyTop = !(m_Camera_Top == top);
    if (isDirtyTop)
    {
        m_Camera_Top.x = top[0];
        m_Camera_Top.y = top[1];
        m_Camera_Top.z = top[2];
    }

    bool isDirty = (isDirtyPosition || isDirtyFront || isDirtyTop);
    if (isDirty)
        emit cameraChanged(isDirtyPosition,isDirtyFront,isDirtyTop);

    return isDirty;
}

void TsVrObjSelf::resetCamera()
{
    m_Camera_Pos.x = 0.0f;
    m_Camera_Pos.y = 0.0f;
    m_Camera_Pos.z = 0.0f;

    m_Camera_Front.x = 0.0f;
    m_Camera_Front.y = 0.0f;
    m_Camera_Front.z = 0.0f;

    m_Camera_Top.x = 0.0f;
    m_Camera_Top.y = 0.0f;
    m_Camera_Top.z = 0.0f;
}
