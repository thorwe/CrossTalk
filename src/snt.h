#pragma once

#include "core/definitions.h"
#include "core/ts_error.h"
#include "core/ts_serversinfo.h"

#include "teamspeak/public_definitions.h"

#include <QtCore/QObject>

#include <string>
#include <string_view>

class SnT : public QObject
{
    Q_OBJECT

public:
    explicit SnT(TSServersInfo& servers_info, QObject* parent = nullptr);

    void
    ParseCommand(com::teamspeak::connection_id_t connection_id, const QString &cmd, const QStringList &args);
    void onClientSelfVariableUpdateEvent(com::teamspeak::connection_id_t connection_id,
                                         int flag,
                                         const char *oldValue,
                                         const char *newValue);
    std::string getReturnCode() const;
    void onServerError(com::teamspeak::connection_id_t connection_id,
                       std::string_view error_message,
                       ts_errc error,
                       std::string_view return_code,
                       std::string_view extra_message);

  public slots:
    void PttDelayFinished();

private:
  static inline GroupWhisperTargetMode GetGroupWhisperTargetMode(const QString &val);
  static inline GroupWhisperType GetGroupWhisperType(const QString &val);

  TSServersInfo &m_servers_info;

  bool m_shallActivatePtt = false;

  com::teamspeak::connection_id_t m_returnToSCHandler = 0;
  bool m_shallClearWhisper = false;
  std::string m_return_code;
  com::teamspeak::connection_id_t m_returnCodeScHandler = 0;

  // short term fix for double trigger
  QString m_last_cmd = QString::null;
};
