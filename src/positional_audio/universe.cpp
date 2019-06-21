#include "universe.h"

#include "core/ts_logging_qt.h"
#include "teamspeak/clientlib_publicdefinitions.h"

Universe::Universe(QObject *parent) :
    QObject(parent)
{

}

//! Create and add a TsVR struct to the Universe map
/*!
 * \brief Universe::Add Helper function
 * \param serverConnectionHandlerID the connection id of the server
 * \param clientID the client id
 */
TsVR* Universe::Add(uint64 serverConnectionHandlerID,anyID clientID)
{
    TsVR* obj = new TsVR();
    if (!(UniverseMap.contains(serverConnectionHandlerID))) //safety measurement
    {
        QMap<anyID,TsVR*>* ConnectionHandlerUniverse = new QMap<anyID,TsVR*>;
        ConnectionHandlerUniverse->insert(clientID,obj);
        UniverseMap.insert(serverConnectionHandlerID,ConnectionHandlerUniverse);
    }
    else
    {
        QMap<anyID,TsVR*>* ConnectionHandlerUniverse = UniverseMap.value(serverConnectionHandlerID);
        ConnectionHandlerUniverse->insert(clientID,obj);
    }
    return obj;
}

//! When disconnecting from a server tab, clear channel Universe
/*!
 * \brief Universe::onConnectStatusChanged TS Event
 * \param serverConnectionHandlerID the connection id of the server
 * \param newStatus used:STATUS_DISCONNECTED
 * \param errorNumber unused
 */
void Universe::onConnectStatusChanged(uint64 serverConnectionHandlerID, int newStatus, unsigned int errorNumber)
{
    Q_UNUSED(errorNumber);
    if (newStatus == STATUS_DISCONNECTED)
            Remove(serverConnectionHandlerID);
    else if ((newStatus == STATUS_CONNECTION_ESTABLISHED) && !(UniverseMap.contains(serverConnectionHandlerID)))
        UniverseMap.insert(serverConnectionHandlerID, new QMap<anyID,TsVR*>);
}

//! Remove a specific object
/*!
 * \brief Universe::Remove Helper function
 * \param serverConnectionHandlerID the connection id of the server
 * \param clientID the client id on the current tab
 */
void Universe::Remove(uint64 serverConnectionHandlerID, anyID clientID)
{
    if (!(UniverseMap.contains(serverConnectionHandlerID)))
        return;

    QMap<anyID,TsVR*>* ConnectionHandlerUniverse = UniverseMap.value(serverConnectionHandlerID);
    if (!(ConnectionHandlerUniverse->contains(clientID)))
        return;

    delete ConnectionHandlerUniverse->take(clientID);
}

//! Remove all TsVR objects of a server tab
/*!
 * \brief Universe::Remove Helper function
 * \param serverConnectionHandlerID the connection id of the server
 */
void Universe::Remove(uint64 serverConnectionHandlerID)
{
    if (!(UniverseMap.contains(serverConnectionHandlerID)))
        return;

    QMap<anyID,TsVR*>* ConnectionHandlerUniverse = UniverseMap.take(serverConnectionHandlerID);
    QMutableMapIterator<anyID,TsVR*> i(*ConnectionHandlerUniverse);
    while (i.hasNext())
    {
        i.next();
        delete ConnectionHandlerUniverse->take(i.key());
    }
    UniverseMap.remove(serverConnectionHandlerID);
}

//! Remove all TsVR objects
/*!
 * \brief Universe::Remove Helper function
 */
void Universe::Remove()
{
    if (UniverseMap.isEmpty())
        return;

    QMutableMapIterator<uint64, QMap<anyID,TsVR*>* > i(UniverseMap);
    while (i.hasNext())
    {
        i.next();
        Remove(i.key());
    }
}

bool Universe::Contains(uint64 serverConnectionHandlerID, anyID clientID)
{
    if (!UniverseMap.contains(serverConnectionHandlerID))
        return false;
    return UniverseMap.value(serverConnectionHandlerID)->contains(clientID);
}

TsVR* Universe::Get(uint64 serverConnectionHandlerID, anyID clientID)
{
    if (!UniverseMap.contains(serverConnectionHandlerID))
        return NULL;
    if (!UniverseMap.value(serverConnectionHandlerID)->contains(clientID))
        return NULL;
    return UniverseMap.value(serverConnectionHandlerID)->value(clientID);
}
