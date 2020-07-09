#include "tsvr_obj_self.h"

TsVrObjSelf::TsVrObjSelf(QObject *parent)
{
    this->setParent(parent);
}

thorwe::Three_Dee_Info TsVrObjSelf::get_camera() const
{
    return m_camera;
}

void TsVrObjSelf::set_avatar(const thorwe::Three_Dee_Info &value)
{
    m_avatar = value;
}

void TsVrObjSelf::set_camera(const thorwe::Three_Dee_Info &value)
{
    m_camera = value;
}

void TsVrObjSelf::reset_camera()
{
    m_camera.position = {0.f, 0.f, 0.f};
    m_camera.front = {0.f, 0.f, 0.f};
    m_camera.top = {0.f, 0.f, 0.f};
}
