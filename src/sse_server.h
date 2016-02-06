#ifndef SSE_SERVER_H
#define SSE_SERVER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QTimer>

class SseServer : public QTcpServer
{
    Q_OBJECT
public:
    explicit SseServer(QObject *parent = 0, quint16 port = 64736);
//    void pause();
//    void resume();
    void Send(QTcpSocket* socket, QString val);
    void Send(QString val);

signals:

public slots:

private slots:
    void readClient();
    void discardClient();

    void onNewConnection();
    void onKeepAlive();

private:
//    void incomingConnection(int socket);
    bool m_isEnabled = true;

    QSet<QTcpSocket*>* m_Sockets;
    QMap<QTcpSocket*,QTextStream*> m_SocketStreams;
    QTimer* m_keepAlive;
};

#endif // SSE_SERVER_H
