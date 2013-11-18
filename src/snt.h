#ifndef SNT_H
#define SNT_H

#include <QObject>
#include "public_definitions.h"

class SnT : public QObject
{
    Q_OBJECT

public:
    explicit SnT(QObject *parent = 0);
    void ParseCommand(uint64 serverConnectionHandlerID, QString cmd, QStringList args);
    void onClientSelfVariableUpdateEvent(uint64 serverConnectionHandlerID, int flag, const char* oldValue, const char* newValue);

signals:
    
public slots:

    void PttDelayFinished();

private:

    static inline GroupWhisperTargetMode GetGroupWhisperTargetMode(QString val);
    static inline GroupWhisperType GetGroupWhisperType(QString val);

    bool m_shallActivatePtt;

    uint64 m_returnToSCHandler;
    bool m_shallClearWhisper;
};

#endif // SNT_H
