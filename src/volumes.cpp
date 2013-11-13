#include "volumes.h"

#include "ts_logging_qt.h"

#include "dsp_volume_ducker.h"
#include "dsp_volume_agmu.h"

Volumes::Volumes(QObject *parent, VolumeType volumeType) :
    QObject(parent)
{
    this->setObjectName("Volumes");
    m_VolumeType = volumeType;
    VolumesMap = new QMap<uint64, QMap<anyID,DspVolume*>* >;
}

//! Create and add a Volume object to the Volumes map
/*!
 * \brief Volumes::AddVolume Helper function
 * \param serverConnectionHandlerID the connection id of the server
 * \param clientID the client id
 */
DspVolume* Volumes::AddVolume(uint64 serverConnectionHandlerID,anyID clientID)
{
    DspVolume* dspObj;
    if (m_VolumeType == VolumeTypeDucker)
        dspObj = new DspVolumeDucker(this);
    else if (m_VolumeType == VolumeTypeAGMU)
        dspObj = new DspVolumeAGMU(this);
    else
        dspObj = new DspVolume(this);

    if (!(VolumesMap->contains(serverConnectionHandlerID)))
    {
        QMap<anyID,DspVolume*>* ConnectionHandlerVolumes = new QMap<anyID,DspVolume*>;
        ConnectionHandlerVolumes->insert(clientID,dspObj);
        VolumesMap->insert(serverConnectionHandlerID,ConnectionHandlerVolumes);
    }
    else
    {
        QMap<anyID,DspVolume*>* ConnectionHandlerVolumes = VolumesMap->value(serverConnectionHandlerID);
        ConnectionHandlerVolumes->insert(clientID,dspObj);
    }
    return dspObj;
}

//! When disconnecting from a server tab, clear channel volumes
/*!
 * \brief Volumes::onConnectStatusChanged TS Event
 * \param serverConnectionHandlerID the connection id of the server
 * \param newStatus used:STATUS_DISCONNECTED
 * \param errorNumber unused
 */
void Volumes::onConnectStatusChanged(uint64 serverConnectionHandlerID, int newStatus, unsigned int errorNumber)
{
    Q_UNUSED(errorNumber);
    if (newStatus==STATUS_DISCONNECTED)
        RemoveVolumes(serverConnectionHandlerID);
}

//! Prepare and schedule deletion of a DspVolume object
/*!
 * \brief Volumes_Global::DeleteVolume Helper function
 * \param dspObj the DspVolume object to delete
 */
void Volumes::DeleteVolume(DspVolume *dspObj)
{
    dspObj->parent()->disconnect(dspObj);
    this->parent()->disconnect(dspObj);

    if (m_VolumeType == VolumeTypeDucker)   //should be unnecessary
        ((DspVolumeDucker*)dspObj)->setGainAdjustment(false);

    dspObj->blockSignals(true);
    dspObj->deleteLater();
}

//! Remove a specific Volume object from the ServerChannelVolumes map
/*!
 * \brief Volumes::RemoveVolume Helper function
 * \param serverConnectionHandlerID the connection id of the server
 * \param clientID the client id on the current tab
 */
void Volumes::RemoveVolume(uint64 serverConnectionHandlerID, anyID clientID)
{
    if (!(VolumesMap->contains(serverConnectionHandlerID)))
        return;

    QMap<anyID,DspVolume*>* ConnectionHandlerVolumes = VolumesMap->value(serverConnectionHandlerID);
    if (!(ConnectionHandlerVolumes->contains(clientID)))
        return;

    DspVolume* dspObj = ConnectionHandlerVolumes->take(clientID);
    DeleteVolume(dspObj);
}

//! Remove all Volume objects from a servers Volumes map
/*!
 * \brief Volumes::RemoveVolumes Helper function
 * \param serverConnectionHandlerID the connection id of the server
 */
void Volumes::RemoveVolumes(uint64 serverConnectionHandlerID)
{
    if (!(VolumesMap->contains(serverConnectionHandlerID)))
        return;

    QMap<anyID,DspVolume*>* ConnectionHandlerVolumes = VolumesMap->take(serverConnectionHandlerID);
    QMutableMapIterator<anyID,DspVolume*> i(*ConnectionHandlerVolumes);
    while (i.hasNext())
    {
        i.next();
        DspVolume* dspObj = ConnectionHandlerVolumes->take(i.key());
        DeleteVolume(dspObj);
    }
    VolumesMap->remove(serverConnectionHandlerID);
    //TSLogging::Log("Volumes: Server Volumes cleared",serverConnectionHandlerID,LogLevel_INFO);
}

//! Remove all Volume objects from all servers Volumes map
/*!
 * \brief Volumes::RemoveVolumes Helper function
 */
void Volumes::RemoveVolumes()
{
    if (VolumesMap->isEmpty())
        return;

    QMutableMapIterator<uint64, QMap<anyID,DspVolume*>* > i(*VolumesMap);
    while (i.hasNext())
    {
        i.next();
        RemoveVolumes(i.key());
    }
}

bool Volumes::ContainsVolume(uint64 serverConnectionHandlerID, anyID clientID)
{
    if (!VolumesMap->contains(serverConnectionHandlerID))
        return false;
    return VolumesMap->value(serverConnectionHandlerID)->contains(clientID);
}

DspVolume* Volumes::GetVolume(uint64 serverConnectionHandlerID, anyID clientID)
{
    if (!VolumesMap->contains(serverConnectionHandlerID))
        return NULL;
    if (!VolumesMap->value(serverConnectionHandlerID)->contains(clientID))
        return NULL;
    return VolumesMap->value(serverConnectionHandlerID)->value(clientID);
}
