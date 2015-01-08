#ifndef PIPESERVER_H
#define PIPESERVER_H

#include <QObject>
#include <QLocalSocket>

class QLocalServer;
class QSignalMapper;

class PipeServer : public QObject
{
    Q_OBJECT

public:
    explicit PipeServer(QObject *parent = 0, QString name = "ThorweCtPipeServer");

signals:

public slots:
    void Send(QString message);

private slots:
    void onNewConnection();
    void onClientDisconnected(QString descStr);

private:
    QLocalServer *m_PipeServer;
    QMap<QString, QLocalSocket*> m_Clients;
    QSignalMapper *m_SignalMapper;
};

#endif // PIPESERVER_H
