#ifndef TSFUNCTIONS_H
#define TSFUNCTIONS_H

#include "public_errors.h"
#include "public_errors_rare.h"
#include "public_definitions.h"
#include "public_rare_definitions.h"
#include "ts3_functions.h"

#include <QObject>
#include <QtCore>
//#include <QMutex>

#include "ts_settings_qt.h"

class TSFunctions : public QObject
{
    Q_OBJECT

public:
    static TSFunctions* instance() {
        static QMutex mutex;
        if(!m_Instance) {
            mutex.lock();

            if(!m_Instance)
                m_Instance = new TSFunctions;

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

    unsigned int GetClientId(uint64 serverConnectionHandlerID, anyID *result);
    void onConnectStatusChangeEvent(uint64 serverConnectionHandlerID, int newStatus, unsigned int errorNumber);

//    int ParseCommand(uint64 serverConnectionHandlerID, const char *cmd);

    bool InitLocalization();
    void ShutdownLocalization();
    QTranslator* translator;

signals:
    void Command(uint64 serverConnectionHandlerID, QString cmd, QStringList args);

public slots:

private:
    explicit TSFunctions();
    ~TSFunctions();
    static TSFunctions* m_Instance;
    TSFunctions(const TSFunctions &);
    TSFunctions& operator=(const TSFunctions &);

//    QMutex* command_mutex;

    QMap<uint64,anyID> myClientIDs;
};

#endif // TSFUNCTIONS_H
