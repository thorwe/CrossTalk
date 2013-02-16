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
    void Print(QString message, uint64 serverConnectionHandlerID, LogLevel logLevel);
    inline void Print(QString message, LogLevel logLevel)   {Print(message, 0 ,logLevel);}
    inline void Print(QString message)                      {Print(message, 0, LogLevel_INFO);}
    void Log(QString message, uint64 serverConnectionHandlerID, LogLevel logLevel);
    inline void Log(QString message, LogLevel logLevel)     {Log(message, 0, logLevel);}
    inline void Log(QString message)                        {Log(message, 0, LogLevel_INFO);}
    void Error(QString message, uint64 serverConnectionHandlerID, unsigned int error);
    inline void Error(QString message, unsigned int error)  {Error(message, 0, error);}
    inline void Error(QString message)                      {Error(message, 0, NULL);}
    bool m_isPrintEnabled;
private:
    bool m_enabled;
    bool m_blocked;
    bool m_isInit;
};

#endif // MODULE_H
