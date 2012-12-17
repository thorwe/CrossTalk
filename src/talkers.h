#ifndef TALKERS_H
#define TALKERS_H

#include <QObject>
#include "public_definitions.h"
#include "tsfunctions.h"

class Talkers : public QObject
{
    Q_OBJECT

public:
    static Talkers* instance() {
        static QMutex mutex;
        if(!m_Instance) {
            mutex.lock();

            if(!m_Instance)
                m_Instance = new Talkers;

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
    
    void onTalkStatusChangeEvent(uint64 serverConnectionHandlerID, int status, int isReceivedWhisper, anyID clientID);
    void onConnectStatusChangeEvent(uint64 serverConnectionHandlerID, int newStatus, unsigned int errorNumber);

    QMap<uint64, QMap<anyID, bool> *>* GetTalkersMap() const;

    unsigned int RefreshTalkers(uint64 serverConnectionHandlerID);
    unsigned int RefreshAllTalkers();

signals:
    void TalkStatusChanged(uint64 serverConnectionHandlerID, int status, bool isReceivedWhisper, anyID clientID);
    void ConnectStatusChanged(uint64 serverConnectionHandlerID, int newStatus, unsigned int errorNumber);
public slots:
    
private:
    explicit Talkers();
    ~Talkers();
    static Talkers* m_Instance;
    Talkers(const Talkers &);
    Talkers& operator=(const Talkers &);
    QMutex* mutex;

    TSFunctions *ts;

    void ProcessClientMove(uint64 serverConnectionHandlerID, anyID clientID, uint64 newChannelID);

    QMap<uint64,QMap<anyID,bool>* >* TalkersMap;
};

#endif // TALKERS_H
