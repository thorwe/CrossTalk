#ifndef SNT_H
#define SNT_H

#include <QObject>
#include "tsfunctions.h"

class SnT : public QObject
{
    Q_OBJECT

public:
    explicit SnT(QObject *parent = 0);
    
    void onClientSelfVariableUpdateEvent(uint64 serverConnectionHandlerID, int flag, const char* oldValue, const char* newValue);

signals:
    
public slots:
    void ParseCommand(char *cmd, char *arg);
    void PttDelayFinished();

private:
    TSFunctions *ts;

    bool m_shallActivatePtt;

    uint64 m_returnToSCHandler;
    bool m_shallClearWhisper;
};

#endif // SNT_H
