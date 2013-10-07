#ifndef UPDATER_H
#define UPDATER_H

#include <QObject>
#include <QtNetwork/QtNetwork>
//#include <QAbstractButton>
//#include <QMessageBox>

class Updater : public QObject
{
    Q_OBJECT
public:
    explicit Updater(QObject *parent = 0);
    
signals:
    
public slots:
    void onNetwManagerFinished(QNetworkReply *reply);
    void CheckUpdate(bool isBetaChannelEnabled);
//    void onButtonClicked(QAbstractButton * button);

private:
    QNetworkAccessManager *m_netwManager;

//    QMessageBox* updateMsgBox;
    void ShowUpdateDialog(QString remoteVersion);
    void CheckUpdate(QUrl url);

    QUrl _urlRedirectedTo;
    QUrl redirectUrl(const QUrl& possibleRedirectUrl, const QUrl& oldRedirectUrl) const;

    void CheckTriggerUpdateDialog();
    QSet<QUrl> m_URLs;
    QString m_VersionStable;
    QString m_VersionBeta;
    QUrl m_downloadUrl;
    bool m_isBetaChannel;
};

#endif // UPDATER_H
