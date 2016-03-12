#ifndef UPDATER_H
#define UPDATER_H

#include <QObject>
#include <QtNetwork/QtNetwork>

class Updater : public QObject
{
    Q_OBJECT
public:
    explicit Updater(QObject *parent = 0);
    
signals:
    
public slots:
    void onNetwManagerFinished(QNetworkReply *reply);
    void CheckUpdate(bool isBetaChannelEnabled);

private:
    QNetworkAccessManager *m_netwManager = NULL;

    void ShowUpdateDialog(QString remoteVersion, QUrl downloadUrl);
    void CheckUpdate(QUrl url);

    QUrl _urlRedirectedTo;
    QUrl redirectUrl(const QUrl& possibleRedirectUrl, const QUrl& oldRedirectUrl) const;

    void parseResponse(QNetworkReply *reply);
    void parseGithubResponse(QNetworkReply *reply);
    void CheckTriggerUpdateDialog();
    QSet<QUrl> m_URLs;
    QString m_VersionStable;
    QString m_VersionBeta;

    QString m_VersionGithubBeta;
    QUrl m_GithubBetaDownloadUrl;

    QUrl m_resultDownloadUrl;
    bool m_isBetaChannel = false;
};

#endif // UPDATER_H
