#ifndef TSVR_OBJ_SELF_H
#define TSVR_OBJ_SELF_H

#include <QObject>
#include "tsvr_obj.h"

class TsVrObjSelf : public TsVrObj
{
    Q_OBJECT
    Q_PROPERTY(TS3_VECTOR cameraPosition
               READ getCameraPosition)
    Q_PROPERTY(TS3_VECTOR cameraFront
               READ getCameraFront)
    Q_PROPERTY(TS3_VECTOR cameraTop
               READ getCameraTop)
public:
    explicit TsVrObjSelf(QObject *parent = 0);
    
    TS3_VECTOR getCameraPosition() const;
    TS3_VECTOR getCameraFront() const;
    TS3_VECTOR getCameraTop() const;

    bool setCamera(const float* position, const float* front, const float* top);
    bool setAvatar(const float* position, const float* front, const float* top);

signals:
    void cameraChanged(bool,bool,bool);

public slots:
    void resetCamera();

private:
    TS3_VECTOR m_Camera_Pos;
    TS3_VECTOR m_Camera_Front;
    TS3_VECTOR m_Camera_Top;
};

#endif // TSVR_OBJ_SELF_H
