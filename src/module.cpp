#include "module.h"
#include "ts3_functions.h"
#include "plugin.h"

#include <QTime>
#include <QTextStream>

#include "ts_logging_qt.h"

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

void Module::Print(QString message, uint64 serverConnectionHandlerID, LogLevel logLevel)
{
#ifndef CONSOLE_OUTPUT
    Q_UNUSED(message);
    Q_UNUSED(serverConnectionHandlerID);
    Q_UNUSED(logLevel);
#else
    if (!m_isPrintEnabled)
        return;

    TSLogging::Print((this->objectName() + ": " + message), serverConnectionHandlerID, logLevel);
#endif
}

void Module::Log(QString message, uint64 serverConnectionHandlerID, LogLevel logLevel)
{
    TSLogging::Log((this->objectName() + ": " + message),serverConnectionHandlerID,logLevel);
    Print(message, serverConnectionHandlerID, logLevel);
}

void Module::Error(QString message, uint64 serverConnectionHandlerID, unsigned int error)
{
    TSLogging::Error((this->objectName() + ": " + message), serverConnectionHandlerID, error);
}
