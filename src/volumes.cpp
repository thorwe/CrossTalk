#include "volumes.h"
#include "clientlib_publicdefinitions.h"
#include "public_errors.h"
#include "public_errors_rare.h"
#include "public_definitions.h"
#include "public_rare_definitions.h"
#include "ts3_functions.h"
#include "plugin.h"

Volumes::Volumes(QObject *parent) :
    QObject(parent)
{
    ts = TSFunctions::instance();
    VolumesMap = new QMap<uint64, QMap<anyID,SimpleVolume*>* >;
}

//! Create and add a Volume object to the Volumes map
/*!
 * \brief Volumes::AddVolume Helper function
 * \param serverConnectionHandlerID the connection id of the server
 * \param clientID the client id
 */
SimpleVolume* Volumes::AddVolume(uint64 serverConnectionHandlerID,anyID clientID)
{
    int type = -1;
    unsigned int error;
    if ((error = ts3Functions.getClientVariableAsInt(serverConnectionHandlerID, clientID, CLIENT_TYPE, &type)) != ERROR_ok)
    {
        ts->Error(serverConnectionHandlerID,error, "(AddVolume) Error checking if client is real.");
        return (SimpleVolume*)NULL;
    }
    if (type != 0)
        return (SimpleVolume*)NULL;

    SimpleVolume* dspObj = new SimpleVolume();
    if (!(VolumesMap->contains(serverConnectionHandlerID)))
    {
        QMap<anyID,SimpleVolume*>* ConnectionHandlerVolumes = new QMap<anyID,SimpleVolume*>;
        ConnectionHandlerVolumes->insert(clientID,dspObj);
        VolumesMap->insert(serverConnectionHandlerID,ConnectionHandlerVolumes);
    }
    else
    {
        QMap<anyID,SimpleVolume*>* ConnectionHandlerVolumes = VolumesMap->value(serverConnectionHandlerID);
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
    if (newStatus==STATUS_DISCONNECTED)
        RemoveVolumes(serverConnectionHandlerID);
}

//! Prepare and schedule deletion of a SimpleVolume object
/*!
 * \brief Volumes_Global::DeleteVolume Helper function
 * \param dspObj the SimpleVolume object to delete
 */
void Volumes::DeleteVolume(SimpleVolume* dspObj)
{
    dspObj->parent()->disconnect(dspObj);
    this->parent()->disconnect(dspObj);
    dspObj->setGainAdjustment(false);
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

    QMap<anyID,SimpleVolume*>* ConnectionHandlerVolumes = VolumesMap->value(serverConnectionHandlerID);
    if (!(ConnectionHandlerVolumes->contains(clientID)))
        return;

    SimpleVolume* dspObj = ConnectionHandlerVolumes->take(clientID);
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

    QMap<anyID,SimpleVolume*>* ConnectionHandlerVolumes = VolumesMap->take(serverConnectionHandlerID);
    QMutableMapIterator<anyID,SimpleVolume*> i(*ConnectionHandlerVolumes);
    while (i.hasNext())
    {
        i.next();
        SimpleVolume* dspObj = ConnectionHandlerVolumes->take(i.key());
        DeleteVolume(dspObj);
    }
    VolumesMap->remove(serverConnectionHandlerID);
    //ts->Log(QString("Volumes: Server Volumes cleared: %1").arg(serverConnectionHandlerID));
}

//! Remove all Volume objects from all servers Volumes map
/*!
 * \brief Volumes::RemoveVolumes Helper function
 */
void Volumes::RemoveVolumes()
{
    if (VolumesMap->isEmpty())
        return;

    QMutableMapIterator<uint64, QMap<anyID,SimpleVolume*>* > i(*VolumesMap);
    while (i.hasNext())
    {
        i.next();
        RemoveVolumes(i.key());
    }
}
