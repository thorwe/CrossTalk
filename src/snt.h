#pragma once

#include <QtCore/QObject>
#include "teamspeak/public_definitions.h"
#include "core/ts_serversinfo.h"

class SnT : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString returnCode READ getReturnCode)

public:
    explicit SnT(TSServersInfo& servers_info, QObject* parent = nullptr);

    void ParseCommand(uint64 serverConnectionHandlerID, QString cmd, QStringList args);
    void onClientSelfVariableUpdateEvent(uint64 serverConnectionHandlerID, int flag, const char* oldValue, const char* newValue);
    QString getReturnCode() const;
    void onServerError(uint64 serverConnectionHandlerID, const char* errorMessage, unsigned int error, const char* returnCode, const char* extraMessage);

public slots:
    void PttDelayFinished();

private:
    static inline GroupWhisperTargetMode GetGroupWhisperTargetMode(QString val);
    static inline GroupWhisperType GetGroupWhisperType(QString val);

    TSServersInfo& m_servers_info;

    bool m_shallActivatePtt = false;

    uint64 m_returnToSCHandler = 0;
    bool m_shallClearWhisper = false;
    QString m_returnCode = QString::null;
    uint64 m_returnCodeScHandler = 0;

    // short term fix for double trigger
    QString m_last_cmd = QString::null;
};
