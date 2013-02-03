#ifndef DUCKER_GLOBAL_H
#define DUCKER_GLOBAL_H

#include <QObject>
#include "public_definitions.h"
#include "module.h"
#include "volumes.h"
#include "tsfunctions.h"
#include "talkers.h"

class Ducker_Global : public Module
{
    Q_OBJECT
    Q_PROPERTY(bool isActive
               READ isActive
               WRITE setActive
               NOTIFY activeSet)
    Q_PROPERTY(float value
               READ value
               WRITE setValue
               NOTIFY valueSet)

public:
    explicit Ducker_Global(QObject *parent = 0);
//    static Ducker_Global* instance() {
//        static QMutex mutex;
//        if(!m_Instance) {
//            mutex.lock();

//            if(!m_Instance)
//                m_Instance = new Ducker_Global;

//            mutex.unlock();
//        }
//        return m_Instance;
//    }

//    static void drop() {
//        static QMutex mutex;
//        mutex.lock();
//        delete m_Instance;
//        m_Instance = 0;
//        mutex.unlock();
//    }

    void AddMusicBot(uint64 serverConnectionHandlerID, anyID clientID);
//    void AddMusicBot(QString uid);
    void RemoveMusicBot(uint64 serverConnectionHandlerID, anyID clientID);
//    void RemoveMusicBot(QString uid);
    void ToggleMusicBot(uint64 serverConnectionHandlerID, anyID clientID);

    // events forwarded from plugin.cpp
    void onClientMoveEvent(uint64 serverConnectionHandlerID, anyID clientID, uint64 oldChannelID, uint64 newChannelID, int visibility);
    bool onEditPlaybackVoiceDataEvent(uint64 serverConnectionHandlerID, anyID clientID, short* samples, int sampleCount, int channels);
    bool isClientMusicBot(uint64 serverConnectionHandlerID, anyID clientID);

    bool isActive() {return m_isActive;}
    void setActive(bool); // for testing command, move to private later

    QMap<QString,QString>* DuckTargets;

signals:
    void valueSet(float);
    void activeSet(bool);
public slots:
    void setValue(float newValue);
    void onTalkStatusChanged(uint64 serverConnectionHandlerID, int status, bool isReceivedWhisper, anyID clientID);

protected:
    void onRunningStateChanged(bool value);

private:
//    explicit Ducker_Global();
//    ~Ducker_Global();
//    static Ducker_Global* m_Instance;
//    Ducker_Global(const Ducker_Global &);
//    Ducker_Global& operator=(const Ducker_Global &);

    bool m_isActive;

    float value() {return m_value;}
    float m_value;

    TSFunctions *ts;
    Talkers* talkers;
    Volumes* vols;

    SimpleVolume *AddMusicBotVolume(uint64 serverConnectionHandlerID, anyID clientID);

    void SaveDuckTargets();
};

#endif // DUCKER_GLOBAL_H
