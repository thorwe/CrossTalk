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

#ifndef SERVERTHREADED_H
#define SERVERTHREADED_H

#include <QtCore>
#include <QtNetwork>

#include "QWsServer.h"
#include "QWsSocket.h"
#include "SocketThread.h"

class ServerThreaded : public QObject
{
	Q_OBJECT

    Q_PROPERTY(bool enabled
               READ isEnabled
               WRITE setEnabled
               NOTIFY enabledToggled)
    Q_PROPERTY(quint16 port
               READ getPort
               WRITE setPort
               NOTIFY portChanged)

public:
	ServerThreaded();
	~ServerThreaded();

    bool isEnabled() const;
    quint16 getPort() const;

public slots:
    void setEnabled(bool val);
    void setPort(quint16 val);

	void processNewConnection();

signals:
    void enabledToggled(bool);
    void portChanged(quint16);
	void broadcastMessage(QString message);
    void messageReceived(QString message);

private:
    QtWebsocket::QWsServer* m_Server;

    void start();
    void stop();

    bool m_isEnabled;
    quint16 m_Port;
};

#endif // SERVERTHREADED_H
