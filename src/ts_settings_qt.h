/* Encapsulates querying the TS3 Settings Database via QtSQL
 *
 * Author: Thorsten Weinz
 *
 * credits @ Jules Blok (jules@aerix.nl),
 * see
 * https://github.com/Armada651/g-key
 * for the original std approach
 */

#ifndef TS_SETTINGS_QT_H
#define TS_SETTINGS_QT_H

#include <QtSql>
#include <QMutex>

class TSSettings
{

public:

    static TSSettings* instance() {
        static QMutex mutex;
        if(!m_Instance) {
            mutex.lock();

            if(!m_Instance)
                m_Instance = new TSSettings;

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

    void Init(QString tsConfigPath);

    bool GetSoundPack(QString& result);
    bool GetIconPack(QString& result);
    bool GetDefaultCaptureProfile(QString& result);
    bool GetPreProcessorData(QString profile, QString& result);

    bool GetBookmarks(QStringList &result);
    bool GetBookmarkByServerUID(QString sUID, QMap<QString,QString> &result);
    bool GetContacts(QStringList &result);
//    bool GetContactFromUniqueId(const char *clientUniqueID, QMap<QString, QString> result); //TODO

    bool GetLanguage(QString &result);

    bool Is3DSoundEnabled(bool &result);
    bool Set3DSoundEnabled(bool val);

    QSqlError GetLastError();

private:
    //singleton
    explicit TSSettings();
    ~TSSettings() = default;
    static TSSettings* m_Instance;
    TSSettings(const TSSettings &);
    TSSettings& operator=(const TSSettings &);

    QMap<QString,QString> GetMapFromValue(QString value);
    bool GetValueFromQuery(QString query, QString &result, bool isEmptyValid);
    bool GetValuesFromQuery(QString query, QStringList &result);
    void SetError(QString in);   //create Custom SQL Error Helper
    QSqlError error_qsql;

    QSqlDatabase m_SettingsDb;
};

#endif // TS_SETTINGS_QT_H
