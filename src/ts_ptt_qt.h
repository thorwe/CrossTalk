#ifndef TS_PTT_QT_H
#define TS_PTT_QT_H

#include <QObject>
#include <QtCore>
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

    bool pttActive;
    bool vadActive;
    bool inputActive;

    void UpdatePttDelayInfo();

    bool pttDelayEnabled;
    int pttDelayMsec;
};

#endif // TS_PTT_QT_H
