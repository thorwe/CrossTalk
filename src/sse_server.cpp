#include "sse_server.h"

#include "ts_logging_qt.h"

SseServer::SseServer(QObject *parent, quint16 port) :
    QTcpServer(parent)
{
    this->setObjectName("SseServer");
//    setMaxPendingConnections(2);
    listen(QHostAddress::LocalHost, port);
    connect(this,SIGNAL(newConnection()),SLOT(onNewConnection()));
    m_Sockets = new QSet<QTcpSocket*>;
    m_keepAlive = new QTimer(this);
    m_keepAlive->setSingleShot(false);
    m_keepAlive->setInterval(2000);
    connect(m_keepAlive, SIGNAL(timeout()), this, SLOT(onKeepAlive()));
    TSLogging::Log(this->objectName() + " started.");
}

void SseServer::Send(QTcpSocket *socket, QString val)
{
    QTextStream os(socket);
    os.setAutoDetectUnicode(true);
    //os << //"HTTP/1.1 200 Ok\r\n"
          //"Content-Type: text/event-stream\r\n"
          //"Cache-Control: no-cache\r\n"
          //"Access-Control-Allow-Origin: *\r\n"
          //"Access-Control-Allow-Headers: Cache-Control, Pragma, Origin, Authorization, Content-Type, X-Requested-With\r\n"
          //"Access-Control-Allow-Methods: GET, OPTIONS\r\n"
          //"\r\n"
     os <<     "data: "
       << val << "\n\n";
    os.flush();
}

void SseServer::Send(QString val)
{
    QSet<QTcpSocket*>::const_iterator i = m_Sockets->constBegin();
    while (i != m_Sockets->constEnd())
    {
        Send(*i,val);
        ++i;
    }
//    QMap<QTcpSocket*,QTextStream*>::const_iterator i = m_SocketStreams.constBegin();
//    while (i != m_SocketStreams.constEnd())
//    {
//        *i.value() << val;
//        ++i;
//    }
    if (m_keepAlive->isActive())
        m_keepAlive->start();
}

void SseServer::discardClient()
{
    QTcpSocket* socket = (QTcpSocket*)sender();
    if (m_Sockets->remove(socket))
    {
        TSLogging::Log("Socket removed from stream set.");
        if (m_Sockets->isEmpty())
            m_keepAlive->stop();
    }


//    m_SocketStreams.remove(socket);
    int socketDesc = socket->socketDescriptor();
    socket->deleteLater();

    TSLogging::Log(QString("Client discarded. Connection closed: %1").arg(socketDesc), LogLevel_INFO);
}

void SseServer::onNewConnection()
{
    QTcpSocket* socket = nextPendingConnection();
    connect(socket, SIGNAL(readyRead()), this, SLOT(readClient()));
    connect(socket, SIGNAL(disconnected()), this, SLOT(discardClient()));
    TSLogging::Log(QString("New connection established: %1").arg(socket->socketDescriptor()), LogLevel_INFO);
}

void SseServer::onKeepAlive()
{
    QSet<QTcpSocket*>::const_iterator i = m_Sockets->constBegin();
    while (i != m_Sockets->constEnd())
    {
        QTextStream os(*i);
        os.setAutoDetectUnicode(true);
        os << ": Dont you die on me\n\n";
        os.flush();
        ++i;
    }
//    TSLogging::Log("Sent KeepAlive.");
}

void SseServer::readClient()
{
    if (!m_isEnabled)
        return;

    // This slot is called when the client sent data to the server. The
    // server looks if it was a get request and sends a very simple HTML
    // document back.
    QTcpSocket* socket = (QTcpSocket*)sender();
    QStringList incoming;
    while (socket->canReadLine())
    {
        QString line = QString(socket->readLine());
        TSLogging::Log(QString("%1: %2").arg(socket->socketDescriptor()).arg(line.left(line.length()-2)));
        incoming.append(line);
    }

    QStringList tokens = incoming[0].split(QRegExp("[ \r\n][ \r\n]*"));
    if (tokens[0] == "GET")
    {
        if (tokens[1].startsWith("/positional_audio/stream"))
        {
            TSLogging::Log("Get request for positional audio stream");
            if (m_Sockets->isEmpty())
                m_keepAlive->start(2000);

            m_Sockets->insert(socket);
            QTextStream os(socket);
            os.setAutoDetectUnicode(true);
            os << "HTTP/1.1 200 Ok\r\n"
                  "Content-Type: text/event-stream\r\n"
                  "Cache-Control: no-cache\r\n"
                  "Access-Control-Allow-Origin: *\r\n"
                  "Access-Control-Allow-Headers: Cache-Control, Pragma, Origin, Authorization, Content-Type, X-Requested-With, Accept\r\n"
                  "Access-Control-Allow-Methods: GET, OPTIONS\r\n"
                  "\r\n";
            os.flush();
//            TSLogging::Log(QString("%1: Short stream sent.").arg(socket->socketDescriptor()));
//            Send(socket, QDateTime::currentDateTime().toString());
            //socket->close();
        }
        else
        {
            QTextStream os(socket);
            os.setAutoDetectUnicode(true);
            if (tokens[1] == "/")
            {
                os << "HTTP/1.1 200 Ok\r\n"
                    "Content-Type: text/html; charset=\"utf-8\"\r\n"
                    "\r\n"
                    "<html><body>"
                    "<h1>CrossTalk</h1>\n"
                    //<< QDateTime::currentDateTime().toString() << "\n";
                    "<div id=\"result\">Waiting\n</div>\n"
                    "<script>\n"
                        "if(typeof(EventSource)!==\"undefined\") {\n"
                            "var source=new EventSource(\"http://localhost:" << this->serverPort() <<"/positional_audio/stream\");\n"
                            "source.onmessage=function(event)\n"
                            "{document.getElementById(\"result\").innerHTML+=event.data + \"<br>\";};\n"
                        "}else{document.getElementById(\"result\").innerHTML=\"Sorry, your browser does not support server-sent events...\";}"
                    "</script></body></html>\n";

                TSLogging::Log(QString("Sent Homepage to client %1").arg(socket->socketDescriptor()));
                socket->close();
            }
            else
            {
                os << "HTTP/1.1 404 Not found.";
                TSLogging::Log(QString("Told client %1 404 not found.").arg(socket->socketDescriptor()));
                socket->close();
            }
            //TSLogging::Log("Wrote to client.");
        }
    }
    else if (tokens[0] == "OPTIONS")        // LUCKILY WITH THE PROPER POLYFILL NOT USED :D
    {
        if (tokens[1] == "/positional_audio/stream")
        {
            QTextStream os(socket);
            os.setAutoDetectUnicode(true);
            os << "HTTP/1.1 200 Ok\r\n"
                  //"Content-Type: text/event-stream\r\n"
                  //"Cache-Control: no-cache\r\n"
                  "Access-Control-Allow-Origin: *\r\n"
                  "Access-Control-Allow-Headers: Cache-Control, Pragma, Origin, Authorization, Content-Type, X-Requested-With, Accept\r\n"
                  "Access-Control-Allow-Methods: GET, OPTIONS\r\n"
                  "Access-Control-Allow-Credentials: true\r\n"
                  "Allow: GET, HEAD, OPTIONS\r\n"
                  "Connection: close\r\n"
                  "\r\n";
            os.flush();
            TSLogging::Log(QString("%1: Reply to OPTIONS for %2 sent.").arg(socket->socketDescriptor()).arg(tokens[1]));
        }
    }

    if (socket->state() == QTcpSocket::UnconnectedState)
    {
        if (m_Sockets->remove(socket))
        {
            TSLogging::Log("Socket removed from stream set.");
            if (m_Sockets->isEmpty())
                m_keepAlive->stop();
        }
//                m_SocketStreams.remove(socket);
        int socketDesc = socket->socketDescriptor();
        delete socket;

        TSLogging::Log(QString("Connection closed: %1").arg(socketDesc));
    }
}

//void SseServer::incomingConnection(int socket)
//{
//    if (!m_isEnabled)
//        return;

//    // When a new client connects, the server constructs a QTcpSocket and all
//    // communication with the client is done over this QTcpSocket. QTcpSocket
//    // works asynchronously, this means that all the communication is done
//    // in the two slots readClient() and discardClient().
//    QTcpSocket* s = new QTcpSocket(this);
//    connect(s, SIGNAL(readyRead()), this, SLOT(readClient()));
//    connect(s, SIGNAL(disconnected()), this, SLOT(discardClient()));
//    s->setSocketDescriptor(socket);

//    addPendingConnection(s);

////    m_Sockets.insert(s);
////    QTextStream* os = new QTextStream(s);
////    os->setAutoDetectUnicode(true);
////    m_SocketStreams.insert(s,os);

//    TSLogging::Log(QString("Incoming connection: %1").arg(socket));
////    emit newConnection();
//}
