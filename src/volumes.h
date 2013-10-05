#ifndef VOLUMES_H
#define VOLUMES_H

#include <QObject>
#include <QMap>
#include "public_definitions.h"
//#include "simple_volume.h"
#include "dsp_volume.h"

enum VolumeType {
    VolumeTypeManual = 0,
    VolumeTypeDucker,
    VolumeTypeAGMU
};

class Volumes : public QObject
{
    Q_OBJECT
public:
    explicit Volumes(QObject *parent = 0, VolumeType volumeType = VolumeTypeManual);

    DspVolume* AddVolume(uint64 serverConnectionHandlerID, anyID clientID);
    void DeleteVolume(DspVolume *dspObj);
    void RemoveVolume(uint64 serverConnectionHandlerID, anyID clientID);
    void RemoveVolumes(uint64 serverConnectionHandlerID);
    void RemoveVolumes();
    bool ContainsVolume(uint64 serverConnectionHandlerID, anyID clientID);
    DspVolume* GetVolume(uint64 serverConnectionHandlerID, anyID clientID);

signals:

public slots:
    void onConnectStatusChanged(uint64 serverConnectionHandlerID, int newStatus, unsigned int errorNumber);
protected:

private:
    QMap<uint64, QMap<anyID,DspVolume*>* >* VolumesMap;
    VolumeType m_VolumeType;
};

#endif // VOLUMES_H
