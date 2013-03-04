#ifndef VOLUMES_H
#define VOLUMES_H

#include <QObject>
#include <QMap>
#include "public_definitions.h"
#include "simple_volume.h"

class Volumes : public QObject
{
    Q_OBJECT
public:
    explicit Volumes(QObject *parent = 0);
    QMap<uint64, QMap<anyID,SimpleVolume*>* >* VolumesMap;
    SimpleVolume* AddVolume(uint64 serverConnectionHandlerID, anyID clientID);
    void DeleteVolume(SimpleVolume *dspObj);
    void RemoveVolume(uint64 serverConnectionHandlerID, anyID clientID);
    void RemoveVolumes(uint64 serverConnectionHandlerID);
    void RemoveVolumes();
    bool ContainsVolume(uint64 serverConnectionHandlerID, anyID clientID);
    SimpleVolume* GetVolume(uint64 serverConnectionHandlerID, anyID clientID);

signals:

public slots:
    void onConnectStatusChanged(uint64 serverConnectionHandlerID, int newStatus, unsigned int errorNumber);
protected:

private:
};

#endif // VOLUMES_H
