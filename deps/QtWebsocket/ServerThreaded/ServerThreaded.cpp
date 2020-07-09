/*
Copyright 2013 Antoine Lafarge qtwebsocket@gmail.com

This file is part of QtWebsocket.

QtWebsocket is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
any later version.

QtWebsocket is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with QtWebsocket.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "ServerThreaded.h"

#include "core/ts_logging_qt.h"

ServerThreaded::ServerThreaded() {}

bool ServerThreaded::isEnabled() const
{
    return m_isEnabled;
}

void ServerThreaded::setEnabled(bool val)
{
    if (m_isEnabled == val)
        return;

    m_isEnabled = val;
    if (m_Port == 0)
        return;

    if (m_isEnabled)
        start();
    else
        stop();

    emit enabledToggled(m_isEnabled);
}

quint16 ServerThreaded::getPort() const
{
    return m_Port;
}

void ServerThreaded::setPort(quint16 val)
{
    if (m_Port == val)
        return;

    m_Port = val;

    if (m_isEnabled)
    {
        stop();
        start();
    }

    emit portChanged(val);
}

void ServerThreaded::processNewConnection()
{
    TSLogging::Log("Client connected.",LogLevel_INFO);

	// Get the connecting socket
    QtWebsocket::QWsSocket* socket = m_Server->nextPendingConnection();

	// Create a new thread and giving to him the socket
	SocketThread* thread = new SocketThread(socket);
	
	// connect for message broadcast
    //QObject::connect(socket, SIGNAL(frameReceived(QString)), this, SIGNAL(broadcastMessage(QString)));
    QObject::connect(socket, SIGNAL(frameReceived(QString)), this, SIGNAL(messageReceived(QString)));
	QObject::connect(this, SIGNAL(broadcastMessage(QString)), thread, SLOT(sendMessage(QString)));

	// Starting the thread
    thread->start();
}

void ServerThreaded::start()
{
    m_Server = new QtWebsocket::QWsServer(this);
    if (! m_Server->listen(QHostAddress::Any, m_Port))
    {
        TSLogging::Error(QString("Error: Can't launch server: %1").arg(m_Server->errorString()));
        return;
    }
    else
        TSLogging::Log(QString("Server is listening port %1").arg(m_Port), LogLevel_INFO);

    QObject::connect(m_Server, SIGNAL(newConnection()), this, SLOT(processNewConnection()));
}

void ServerThreaded::stop()
{
    if (m_Server != NULL)
    {
        m_Server->close();
        m_Server->deleteLater();
        m_Server = NULL;
    }
}
