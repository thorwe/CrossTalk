#ifndef TALKERS_H
#define TALKERS_H

#include <QObject>
#include <QtCore>
#include "public_definitions.h"

#include "module.h"

class TalkInterface
{
public:
    virtual bool onTalkStatusChanged(uint64 serverConnectionHandlerID, int status, bool isReceivedWhisper, anyID clientID, bool itsMe) = 0;
};
Q_DECLARE_INTERFACE(TalkInterface,"net.thorwe.CrossTalk.TalkInterface/1.0")



class Talkers : public QObject
{
    Q_OBJECT
    Q_PROPERTY(uint64 isMeTalking
               READ isMeTalking)

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

//    enum TsTalker {isOther = 0x00, isMe = 0x01, isPrioritySpeaker = 0x02, isWhispering = 0x04};
//    Q_DECLARE_FLAGS(TsTalkers, TsTalker)
//    Q_FLAGS(TsTalkers)

    bool onTalkStatusChangeEvent(uint64 serverConnectionHandlerID, int status, int isReceivedWhisper, anyID clientID);
    void onConnectStatusChangeEvent(uint64 serverConnectionHandlerID, int newStatus, unsigned int errorNumber);

//    QMap<uint64, QMap<anyID, bool> *>* GetTalkersMap() const;
    QMultiMap<uint64, anyID> GetTalkerMap() const;
    QMultiMap<uint64, anyID> GetWhisperMap() const;
//    QMultiMap<uint64, anyID> GetPrioritySpeakerMap() const; // commented out until I need it
    uint64 isMeTalking() const;

    unsigned int RefreshTalkers(uint64 serverConnectionHandlerID);
    unsigned int RefreshAllTalkers();

    void DumpTalkStatusChanges(QObject* p, int status);

    //    int RegisterEventTalkStatusChange(QObject *p, int priority, bool isRegister);
signals:
    void ConnectStatusChanged(uint64 serverConnectionHandlerID, int newStatus, unsigned int errorNumber);
public slots:
    
private:
    explicit Talkers();
    ~Talkers();
    static Talkers* m_Instance;
    Talkers(const Talkers &);
    Talkers& operator=(const Talkers &);
    QMutex* mutex;

//    void ProcessClientMove(uint64 serverConnectionHandlerID, anyID clientID, uint64 newChannelID);

//    QMap<uint64,QMap<anyID,bool>* >* TalkersMap;
    uint64 m_meTalkingScHandler;
    bool m_meTalkingIsWhisper;

    QMultiMap<uint64,anyID> TalkerMap;
    QMultiMap<uint64,anyID> WhisperMap;

//    QMultiMap<uint64,anyID> PrioritySpeakerMap; // commented out until I need it

//    void FireTalkStatusChange(uint64 serverConnectionHandlerID, int status, bool isReceivedWhisper, anyID clientID);
//    QList<QPointer<QObject> > TalkStatusChangeModules;
};

//Q_DECLARE_OPERATORS_FOR_FLAGS(Talkers::TsTalkers)

#endif // TALKERS_H
