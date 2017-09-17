#pragma once

#include <QtCore/QObject>
#include <QtCore/QtCore>
//#include <QMutex>

#include "teamspeak/public_definitions.h"

// Ptt Enum
enum PTT_CHANGE_STATUS {
    PTT_ACTIVATE = 0,
    PTT_DEACTIVATE,
    PTT_TOGGLE
};

class TSPtt : public QObject
{
    Q_OBJECT
public:
    static TSPtt* instance() {
        static QMutex mutex;
        if(!m_Instance) {
            mutex.lock();

            if(!m_Instance)
                m_Instance = new TSPtt;

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

    void Init(QMutex* mutex_cmd);
    
    int SetPushToTalk(uint64 serverConnectionHandlerID, PTT_CHANGE_STATUS action);
    int SetPushToTalk(uint64 scHandlerID, bool shouldTalk);

signals:
    void PttDelayFinished();

public slots:
    void onPttDelayFinished();

private:
    explicit TSPtt();
    ~TSPtt();
    static TSPtt* m_Instance;
    TSPtt(const TSPtt &);
    TSPtt& operator=(const TSPtt &);

    QMutex* command_mutex;

    QTimer* timer;

    bool pttActive = false;
    bool vadActive = false;
    bool inputActive = false;

    void UpdatePttDelayInfo();

    bool pttDelayEnabled = false;
    int pttDelayMsec = 0;
};
