#include "plugin_qt.h"
#include <QSettings>
#include "ts_logging_qt.h"
#include "ts_helpers_qt.h"

PluginQt* PluginQt::m_Instance = 0;

PluginQt::PluginQt():
    m_isInit(false),
    m_isServerEnabled(false),
    m_serverPort(0),
    m_SseServer(NULL)
{}

PluginQt::~PluginQt(){}

void PluginQt::Init()
{
    if (m_isInit)
        return;

    QSettings cfg(TSHelpers::GetFullConfigPath(), QSettings::IniFormat);
    bool ok;
    quint16 port = cfg.value("server_port",64736).toUInt(&ok);
    if (!ok)
        TSLogging::Error("Could not read port from settings");
    else
    {
        setServerPort(port);
        setServerEnabled(cfg.value("server_enabled",false).toBool());
    }

//#ifdef USE_QT_WEB_APP
//    m_HttpServer = new TsHttpServer();
//    m_HttpServer->init();
//#endif

    m_isInit = true;
}

bool PluginQt::isServerEnabled() const
{
    return m_isServerEnabled;
}

quint16 PluginQt::getServerPort() const
{
    return m_serverPort;
}

void PluginQt::LocalServerSend(QString val)
{
    if (m_isServerEnabled && m_serverPort > 0)
    {
        m_SseServer->Send(val);
    }
}

void PluginQt::setServerEnabled(bool val)
{
    if (val == m_isServerEnabled)
        return;

    m_isServerEnabled = val;

    if (m_isServerEnabled && m_serverPort > 0)
    {
        serverStart();
    }
    else if (!m_isServerEnabled)
        serverStop();
}

void PluginQt::setServerPort(quint16 val)
{
    if (val == m_serverPort)
        return;

    m_serverPort = val;

    serverStop();
    if (m_isServerEnabled)
    {
        serverStart();
    }
}

void PluginQt::serverStart()
{
    m_SseServer = new SseServer(this,m_serverPort);
    TSLogging::Log(QString("SseServer started on %1").arg(m_serverPort));
}

void PluginQt::serverStop()
{
    if (m_SseServer != NULL)
    {
        m_SseServer->close();
        m_SseServer->deleteLater();
        m_SseServer = NULL;
        TSLogging::Log("SseServer stopped.");
    }
}
