#include "module.h"
#include "ts3_functions.h"
#include "plugin.h"

#include <QTime>
#include <QTextStream>

//Module::Module(QObject *parent) :
//    QObject(parent)
//{
//}

bool Module::isEnabled() const
{
    return m_enabled;
}

bool Module::isBlocked() const
{
    return m_blocked;
}

bool Module::isRunning() const
{
    return (m_enabled && !m_blocked);
}


void Module::setEnabled(bool value)
{
    if (value!=m_enabled)
    {
        bool running_old = isRunning();
        m_enabled = value;
        onEnabledStateChanged(value);
        if (running_old != isRunning())
            onRunningStateChanged(isRunning());
        emit enabledSet(m_enabled);
    }
}

void Module::setBlocked(bool value)
{
    if (value!=m_blocked)
    {
        bool running_old = isRunning();
        m_blocked = value;
        onBlockedStateChanged(value);
        if (running_old != isRunning())
            onRunningStateChanged(isRunning());
        emit blockedSet(value);
    }
}

void Module::Print(QString message, LogLevel logLevel, uint64 serverConnectionHandlerID)
{
    if (!(CONSOLE_OUTPUT))
        return;

    if (!m_isPrintEnabled)
        return;

    if (serverConnectionHandlerID != 0)
        message.prepend(QString("%1 : ").arg(serverConnectionHandlerID));

    message.prepend(QString("%1: ").arg(this->objectName()));

    switch (logLevel)
    {
    case LogLevel_CRITICAL:
        message.prepend("Critical: ");
        break;
    case LogLevel_DEBUG:
        message.prepend("Debug: ");
        break;
    case LogLevel_DEVEL:
        message.prepend("Devel: ");
        break;
    case LogLevel_ERROR:
        message.prepend("Error: ");
        break;
    case LogLevel_INFO:
        message.prepend("Info: ");
        break;
    case LogLevel_WARNING:
        message.prepend("Warning: ");
        break;
    }
    QTime time = QTime::currentTime ();
    QString time_qstr = time.toString(Qt::TextDate);
    QString styledQstr;
    QTextStream(&styledQstr) << time_qstr << ": " << PLUGIN_NAME << ": " << message << "\n";
    printf(styledQstr.toLocal8Bit().constData());
}

void Module::Print(QString message, LogLevel logLevel)
{
    Print(message,logLevel,0);
}

void Module::Print(QString message)
{
    Print(message,LogLevel_INFO,0);
}

void Module::Log(QString message, LogLevel logLevel, uint64 serverConnectionHandlerID)
{
    ts3Functions.logMessage((QString("%1: %2").arg(this->objectName()).arg(message)).toLocal8Bit().constData(), logLevel, PLUGIN_NAME, serverConnectionHandlerID);
    Print(message,logLevel,serverConnectionHandlerID);
}

void Module::Log(QString message, LogLevel logLevel)
{
    Log(message, logLevel, 0);
}

void Module::Log(QString message)
{
    Log(message, LogLevel_INFO);
}
