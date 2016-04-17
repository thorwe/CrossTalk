#include "pipeserver.h"

#include <QtNetwork>
#include <QSignalMapper>
#include "ts_logging_qt.h"

PipeServer::PipeServer(QObject *parent, QString name) :
    QObject(parent)
{
    m_SignalMapper = new QSignalMapper(this);

    m_PipeServer = new QLocalServer(this);
    if (!m_PipeServer->listen(name)) {   //QString(ts3plugin_author()) + QString(ts3plugin_name()))
        TSLogging::Error(QString("Unable to start the server: %1.").arg(m_PipeServer->errorString()));
        return;
    }
    connect(m_PipeServer, &QLocalServer::newConnection, this, &PipeServer::onNewConnection);
    connect(m_SignalMapper, SIGNAL(mapped(QString)), this, SLOT(onClientDisconnected(QString)));
}

void PipeServer::Send(QString message)
{
    if (m_Clients.isEmpty())
        return;

    foreach (QLocalSocket* clientConnection, m_Clients)
        clientConnection->write(message.toLocal8Bit().constData());
}

void PipeServer::onNewConnection()
{
    auto clientConnection = m_PipeServer->nextPendingConnection();
    auto desc = clientConnection->socketDescriptor();
    auto descStr = QString("%1").arg(desc);
    m_Clients.insert(descStr, clientConnection);
    connect(clientConnection, SIGNAL(disconnected()), m_SignalMapper, SLOT(map()));
    m_SignalMapper->setMapping(clientConnection, descStr);
}

void PipeServer::onClientDisconnected(QString descStr)
{
    auto clientConnection = m_Clients.take(descStr);
    clientConnection->deleteLater();
}
