#include "tsvr_universe.h"
#include "clientlib_publicdefinitions.h"

TsVrUniverse::TsVrUniverse(QObject *parent) :
    QObject(parent)
{
}

//! Create and add a TsVR struct to the Universe map
/*!
 * \brief TsVrUniverse::Add Helper function
 * \param serverConnectionHandlerID the connection id of the server
 * \param clientID the client id
 */
TsVrObjOther* TsVrUniverse::Add(uint64 serverConnectionHandlerID,anyID clientID)
{
    TsVrObjOther* obj = new TsVrObjOther(this,serverConnectionHandlerID,clientID);
    if (!(m_Map.contains(serverConnectionHandlerID))) //safety measurement
    {
        QMap<anyID,TsVrObjOther*>* ConnectionHandlerUniverse = new QMap<anyID,TsVrObjOther*>;
        ConnectionHandlerUniverse->insert(clientID,obj);
        m_Map.insert(serverConnectionHandlerID,ConnectionHandlerUniverse);
    }
    else
    {
        QMap<anyID,TsVrObjOther*>* ConnectionHandlerUniverse = m_Map.value(serverConnectionHandlerID);
        ConnectionHandlerUniverse->insert(clientID,obj);
    }
    return obj;
}

//! Remove a specific object
/*!
 * \brief TsVrUniverse::Remove Helper function
 * \param serverConnectionHandlerID the connection id of the server
 * \param clientID the client id on the current tab
 */
void TsVrUniverse::Remove(uint64 serverConnectionHandlerID, anyID clientID)
{
    if (!(m_Map.contains(serverConnectionHandlerID)))
        return;

    QMap<anyID,TsVrObjOther*>* ConnectionHandlerUniverse = m_Map.value(serverConnectionHandlerID);
    if (!(ConnectionHandlerUniverse->contains(clientID)))
        return;

    TsVrObjOther* removedObj = ConnectionHandlerUniverse->take(clientID);
    QString clientUID = removedObj->getClientUID();
    delete removedObj;
    emit removed(clientUID);
}

//! Remove all TsVR objects of a server tab
/*!
 * \brief TsVrUniverse::Remove Helper function
 * \param serverConnectionHandlerID the connection id of the server
 */
void TsVrUniverse::Remove(uint64 serverConnectionHandlerID)
{
    if (!(m_Map.contains(serverConnectionHandlerID)))
        return;

    QMap<anyID,TsVrObjOther*>* ConnectionHandlerUniverse = m_Map.take(serverConnectionHandlerID);
    QMutableMapIterator<anyID,TsVrObjOther*> i(*ConnectionHandlerUniverse);
    while (i.hasNext())
    {
        i.next();
        delete ConnectionHandlerUniverse->take(i.key());
    }
    m_Map.remove(serverConnectionHandlerID);
}

//! Remove all TsVR objects
/*!
 * \brief TsVrUniverse::Remove Helper function
 */
void TsVrUniverse::Remove()
{
    if (m_Map.isEmpty())
        return;

    QMutableMapIterator<uint64, QMap<anyID,TsVrObjOther*>* > i(m_Map);
    while (i.hasNext())
    {
        i.next();
        Remove(i.key());
    }
}

bool TsVrUniverse::Contains(uint64 serverConnectionHandlerID, anyID clientID)
{
    if (!m_Map.contains(serverConnectionHandlerID))
        return false;
    return m_Map.value(serverConnectionHandlerID)->contains(clientID);
}

TsVrObjOther* TsVrUniverse::Get(uint64 serverConnectionHandlerID, anyID clientID)
{
    if (!m_Map.contains(serverConnectionHandlerID))
        return NULL;
    if (!m_Map.value(serverConnectionHandlerID)->contains(clientID))
        return NULL;
    return m_Map.value(serverConnectionHandlerID)->value(clientID);
}

//bool TsVrUniverse::RegisterCustomEnvironmentSupport(QObject *p)
//{
//    CustomEnvironmentSupportInterface *iCustomEnvironmentSupport = qobject_cast<CustomEnvironmentSupportInterface *>(p);
//    if (!iCustomEnvironmentSupport)
//        return false;

//    m_CustomEnvironmentSupportMap.insert(p->objectName(),p);
//    return true;
//}

//! When disconnecting from a server tab, clean up
/*!
 * \brief TsVrUniverse::onConnectStatusChanged TS Event
 * \param serverConnectionHandlerID the connection id of the server
 * \param newStatus used:STATUS_DISCONNECTED
 * \param errorNumber unused
 */
void TsVrUniverse::onConnectStatusChanged(uint64 serverConnectionHandlerID, int newStatus, unsigned int errorNumber)
{
    Q_UNUSED(errorNumber);
    if (newStatus==STATUS_DISCONNECTED)
            Remove(serverConnectionHandlerID);
    else if ((newStatus==STATUS_CONNECTION_ESTABLISHED) && !(m_Map.contains(serverConnectionHandlerID)))
        m_Map.insert(serverConnectionHandlerID, new QMap<anyID,TsVrObjOther*>);
}
