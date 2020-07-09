#pragma once

#include "tsvr_obj.h"

#include <QtCore/QObject>

class TsVrObjSelf : public TsVrObj
{
    Q_OBJECT

  public:
    explicit TsVrObjSelf(QObject *parent = 0);

    thorwe::Three_Dee_Info get_camera() const;

    void set_camera(const thorwe::Three_Dee_Info &);
    void set_avatar(const thorwe::Three_Dee_Info &);

    void reset_camera();

  private:
    thorwe::Three_Dee_Info m_camera;
};
