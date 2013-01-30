#ifndef MODULE_H
#define MODULE_H

#include <QObject>
#include "public_definitions.h"

class Module : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool enabled
               READ isEnabled
               WRITE setEnabled
               NOTIFY enabledSet)
    Q_PROPERTY(bool blocked
               READ isBlocked
               WRITE setEnabled
               NOTIFY blockedSet)
public:
//    explicit Module(QObject *parent = 0);
    bool isEnabled() const;
    bool isBlocked() const;
    bool isRunning() const; // result of enabled and blocked

signals:
    void enabledSet(bool);
    void blockedSet(bool);
public slots:
    void setEnabled(bool value);
    void setBlocked(bool value);
protected:
//    virtual void onEnabledStateChanged(bool) = 0;
//    virtual void onBlockedStateChanged(bool) = 0;
//    virtual void onRunningStateChanged(bool) = 0;
    virtual void onEnabledStateChanged(bool) {}
    virtual void onBlockedStateChanged(bool) {}
    virtual void onRunningStateChanged(bool) {}
    void Print(QString message, LogLevel logLevel, uint64 serverConnectionHandlerID);
    void Print(QString message, LogLevel logLevel);
    void Print(QString message);
    void Log(QString message, LogLevel logLevel, uint64 serverConnectionHandlerID);
    void Log(QString message, LogLevel logLevel);
    void Log(QString message);
    bool m_isPrintEnabled;
private:
    bool m_enabled;
    bool m_blocked;

};

#endif // MODULE_H
