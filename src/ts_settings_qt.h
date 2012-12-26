// Encapsulates querying the TS3 Settings Database via QtSQL
//
// Author: Thorsten Weinz

#ifndef TS_SETTINGS_QT_H
#define TS_SETTINGS_QT_H

#include <QtSql>

class TSSettings
{

public:

    TSSettings();
    
    bool GetSoundPack(QString& result);
    bool GetIconPack(QString& result);
    bool GetDefaultCaptureProfile(QString& result);
    bool GetPreProcessorData(QString profile, QString& result);

    bool GetBookmarks(QStringList &result);
    bool GetBookmarkByServerUID(QString sUID, QMap<QString,QString> &result);
    bool GetContacts(QStringList &result);
//    bool GetContactFromUniqueId(const char *clientUniqueID, QMap<QString, QString> result); //TODO

    bool GetLanguage(QString &result);

    QSqlError GetLastError();

private:
    QMap<QString,QString> GetMapFromValue(QString value);
    bool GetValueFromQuery(QString query, QString &result, bool isEmptyValid);
    bool GetValuesFromQuery(QString query, QStringList &result);
    void SetError(QString in);   //create Custom SQL Error Helper
    QSqlError error_qsql;
};

#endif // TS_SETTINGS_QT_H
