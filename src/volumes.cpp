#include "volumes.h"

#include "ts_logging_qt.h"

#include "dsp_volume_ducker.h"
#include "dsp_volume_agmu.h"

Volumes::Volumes(QObject *parent, Volume_Type volume_type) :
    QObject(parent)
{
    this->setObjectName("Volumes");
    m_volume_type = volume_type;
}

//! Create and add a Volume object to the Volumes map
/*!
 * \brief Volumes::AddVolume Helper function
 * \param serverConnectionHandlerID the connection id of the server
 * \param clientID the client id
 */
DspVolume* Volumes::AddVolume(uint64 serverConnectionHandlerID,anyID clientID)
{
    DspVolume* dsp_obj;
    if (m_volume_type == Volume_Type::DUCKER)
        dsp_obj = new DspVolumeDucker(this);
    else if (m_volume_type == Volume_Type::AGMU)
        dsp_obj = new DspVolumeAGMU(this);
    else
        dsp_obj = new DspVolume(this);

    const auto kKey = qMakePair(serverConnectionHandlerID, clientID);
    if (!m_volumes.contains(kKey))
        m_volumes.insert(kKey, dsp_obj);

    return dsp_obj;
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

    if (m_volume_type == Volume_Type::DUCKER)   //should be unnecessary
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
    const auto kKey = qMakePair(serverConnectionHandlerID, clientID);
    if (!(m_volumes.contains(kKey)))
        return;

    auto dsp_obj = m_volumes.take(kKey);
    DeleteVolume(dsp_obj);
}

//! Remove all Volume objects of a server
/*!
 * \brief Volumes::RemoveVolumes Helper function
 * \param serverConnectionHandlerID the connection id of the server
 */
void Volumes::RemoveVolumes(uint64 serverConnectionHandlerID)
{
    if (m_volumes.isEmpty())
        return;

    for (auto i = m_volumes.begin(); i != m_volumes.end(); ++i)
    {
        if (i.key().first == serverConnectionHandlerID)
        {
            auto dsp_obj = m_volumes.take(i.key());
            DeleteVolume(dsp_obj);
        }
    }
    //TSLogging::Log("Volumes: Server Volumes cleared",serverConnectionHandlerID,LogLevel_INFO);
}

//! Remove all Volume objects
/*!
 * \brief Volumes::RemoveVolumes Helper function
 */
void Volumes::RemoveVolumes()
{
    if (m_volumes.isEmpty())
        return;

    for (auto i = m_volumes.begin(); i != m_volumes.end(); ++i)
        DeleteVolume(i.value());

    m_volumes.clear();
}

bool Volumes::ContainsVolume(uint64 serverConnectionHandlerID, anyID clientID)
{
    const auto kKey = qMakePair(serverConnectionHandlerID, clientID);
    return m_volumes.contains(kKey);
}

DspVolume* Volumes::GetVolume(uint64 serverConnectionHandlerID, anyID clientID)
{
    const auto kKey = qMakePair(serverConnectionHandlerID, clientID);
    return m_volumes.contains(kKey) ? m_volumes[kKey] : nullptr;
}
