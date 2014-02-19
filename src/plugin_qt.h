#ifndef PLUGIN_QT_H
#define PLUGIN_QT_H

#include <QObject>
#include <QMutex>
#include "sse_server.h"
//#ifdef USE_QT_WEB_APP
//  #include "QtWebApp/src/ts_httpserver_qt.h"
//#endif

class PluginQt : public QObject
{
    Q_OBJECT

    // Sse-Server
    Q_PROPERTY(bool serverEnabled
               READ isServerEnabled
               WRITE setServerEnabled
               NOTIFY serverEnabledToggled)
    Q_PROPERTY(quint16 serverPort
               READ getServerPort
               WRITE setServerPort
               NOTIFY serverPortChanged)

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
    bool isServerEnabled() const;
    quint16 getServerPort() const;
    void LocalServerSend(QString val);

//#ifdef USE_QT_WEB_APP
//    TsHttpServer* m_HttpServer;
//#endif

signals:
    // Sse-Server
    void serverEnabledToggled(bool);
    void serverPortChanged(quint16);

public slots:
    // Sse-Server
    void setServerEnabled(bool val);
    void setServerPort(quint16 val);

private:
    //singleton
    explicit PluginQt();
    ~PluginQt();
    static PluginQt* m_Instance;
    PluginQt(const PluginQt &);
    PluginQt& operator=(const PluginQt &);

    bool m_isInit;

    // Sse-Server
    SseServer* m_SseServer;
    bool m_isServerEnabled;
    quint16 m_serverPort;
    inline void serverStart();
    inline void serverStop();

};

#endif // PLUGIN_QT_H
