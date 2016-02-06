#ifndef PLUGIN_QT_H
#define PLUGIN_QT_H

#include <QObject>
#include <QMutex>
#include "sse_server.h"
//#ifdef USE_QT_WEB_APP
//  #include "QtWebApp/src/ts_httpserver_qt.h"
//#endif
#ifdef USE_WEBSOCKET
  #include "ServerThreaded/ServerThreaded.h"
#endif

#include "pipeserver.h"

class PluginQt : public QObject
{
    Q_OBJECT

    // Sse-Server
    Q_PROPERTY(bool sseServerEnabled
               READ isSseServerEnabled
               WRITE setSseServerEnabled
               NOTIFY sseServerEnabledToggled)
    Q_PROPERTY(quint16 sseServerPort
               READ getSseServerPort
               WRITE setSseServerPort
               NOTIFY sseServerPortChanged)

public:
    static PluginQt* instance() {
        static QMutex mutex;
        if(!m_Instance) {
            mutex.lock();

            if(!m_Instance)
                m_Instance = new PluginQt;

            mutex.unlock();
        }
        return m_Instance;
    }

    static void drop() {
        static QMutex mutex;
        mutex.lock();
        delete m_Instance;
        m_Instance = 0;
        mutex.unlock();
    }

    void Init();

    // property getters

    // Sse-Server
    bool isSseServerEnabled() const;
    quint16 getSseServerPort() const;

//#ifdef USE_QT_WEB_APP
//    TsHttpServer* m_HttpServer;
//#endif
#ifdef USE_WEBSOCKET
    ServerThreaded* m_WebSocketServer;
#endif

    PipeServer* m_PipeServer;

signals:
    // Sse-Server
    void sseServerEnabledToggled(bool);
    void sseServerPortChanged(quint16);

public slots:
    // Sse-Server
    void setSseServerEnabled(bool val);
    void setSseServerPort(quint16 val);
    void LocalServerSend(QString val);

private:
    //singleton
    explicit PluginQt();
    ~PluginQt() = default;
    static PluginQt* m_Instance;
    PluginQt(const PluginQt &);
    PluginQt& operator=(const PluginQt &);

    bool m_isInit = false;

    // Sse-Server
    SseServer* m_SseServer = NULL;
    bool m_isServerEnabled = false;
    quint16 m_serverPort = 0;
    inline void serverStart();
    inline void serverStop();

};

#endif // PLUGIN_QT_H
