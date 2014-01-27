#include "updater.h"

//#include <QDialog>

#include <QApplication>
#include <QDesktopServices>
#include <QMessageBox>

#include "ts_helpers_qt.h"
#include "ts_logging_qt.h"

const QUrl STABLE("http://addons.teamspeak.com/directory/plugins/miscellaneous/CrossTalk.html");
const QUrl BETA_CHECK("http://dl.dropbox.com/u/18413693/CrossTalk-builds/package.ini");
const QUrl BETA_DOWNLOAD("http://jianji.de/crosstalk/");

// Note that TS doesn't ship with any ssl support, so no GitHub API etc

//! Constructor
/*!
 * \brief Config::Config create an instance of this class
 */
Updater::Updater(QObject *parent) :
    QObject(parent),
    m_netwManager(NULL)
{
    this->setObjectName("Updater");
}

//! Retrieves update url
/*!
 * \brief Config::onNetwManagerFinished Qt slot to retrieve the content
 * \param reply a QNetworkReply containing the update data to parse
 */
void Updater::onNetwManagerFinished(QNetworkReply *reply)
{
    if (!m_URLs.remove(reply->url()))
        return;

    bool isReplyStable = reply->url().toString().contains("addons.teamspeak.com");
    if (reply->error() != QNetworkReply::NoError)
    {
        TSLogging::Log(reply->errorString(),LogLevel_WARNING);
    }
    else
    {
        QByteArray arr = reply->readAll();

        QVariant possibleRedirectUrl = reply->attribute(QNetworkRequest::RedirectionTargetAttribute);

        /* We'll deduct if the redirection is valid in the redirectUrl function */
        _urlRedirectedTo = this->redirectUrl(possibleRedirectUrl.toUrl(), _urlRedirectedTo);

        if(!_urlRedirectedTo.isEmpty()) /* If the URL is not empty, we're being redirected. */
        {
            TSLogging::Log(QString("%1: Update check forwarding to %2").arg(this->objectName()).arg(_urlRedirectedTo.toString()),LogLevel_INFO);
            CheckUpdate(_urlRedirectedTo);
            return;
        }

        int start = arr.indexOf((isReplyStable)?QString(ts3plugin_name()).append("_"):"Version",0);
        if (start == -1)
        {
            TSLogging::Log((this->objectName() + ": Did not find Version."),LogLevel_WARNING);
            return;
        }

        QString endStr = (isReplyStable)?".ts3_plugin":"Platforms";
        int end = arr.indexOf(endStr,start);
        if (end == -1)
        {
            TSLogging::Log((this->objectName() + ": Did not find %1.").arg(endStr),LogLevel_WARNING);
            return;
        }

        QString parse(arr.mid(start,end-start));
        QRegExp rx;
        if (isReplyStable)
            rx.setPattern("\\d+(?:\\_\\d+)+");
        else
            rx.setPattern("\\d+(?:\\.\\d+)+");

        int pos = rx.indexIn(parse);
        if (pos > -1)
            parse = rx.cap(0);

        if (isReplyStable)
        {
            parse.replace("_",".");
            m_VersionStable = parse;
        }
        else
            m_VersionBeta = parse;
    }

    reply->deleteLater();
    _urlRedirectedTo.clear();
    if (m_URLs.isEmpty())
    {
        m_netwManager->deleteLater();
        m_netwManager=NULL;
        CheckTriggerUpdateDialog();
    }
}

void Updater::CheckUpdate(bool isBetaChannelEnabled)
{
    if (m_netwManager==NULL)
        m_netwManager = new QNetworkAccessManager(this);

//    QSettings cfg(TSHelpers::GetFullConfigPath(), QSettings::IniFormat);
//    m_isBetaChannel = cfg.value("beta",false).toBool();
    m_isBetaChannel = isBetaChannelEnabled;
    if (m_isBetaChannel)
        CheckUpdate(BETA_CHECK);

    CheckUpdate(STABLE);
}

void Updater::CheckUpdate(QUrl url)
{
    m_URLs.insert(url);
    connect(m_netwManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(onNetwManagerFinished(QNetworkReply*)),Qt::UniqueConnection);
    QNetworkRequest request(url);
    m_netwManager->get(request);
}

//void Updater::onButtonClicked(QAbstractButton *button)
//{
////    if ((updateMsgBox->buttonRole(button) == QMessageBox::AcceptRole) || (updateMsgBox->buttonRole(button) == QMessageBox::YesRole))
////        TSLogging::Print("Accepted");
////    else if ((updateMsgBox->buttonRole(button) == QMessageBox::RejectRole) || (updateMsgBox->buttonRole(button) == QMessageBox::NoRole))
////        TSLogging::Print("Rejected");
////    else
////        TSLogging::Print("Could not find button role");

//    //updateMsgBox->deleteLater();
//}

void Updater::ShowUpdateDialog(QString remoteVersion)
{
    QWidget* mainWindow = TSHelpers::GetMainWindow();
    // Create Dialog
    QMessageBox updateMsgBox(mainWindow);
//    updateMsgBox = new QMessageBox();
    updateMsgBox.moveToThread(mainWindow->thread());
//    updateMsgBox->setAttribute( Qt::WA_DeleteOnClose, true ); //makes sure the msgbox is deleted automatically when closed
    updateMsgBox.setIcon(QMessageBox::Question);
    updateMsgBox.setText(QString("An update for %1 is available!").arg(ts3plugin_name()));
    updateMsgBox.setInformativeText(QString("Local Version: %1\nRemote Version: %2\n\nContinue to download site?\n\nNote that due to client limitations, Teamspeak will be closed to enable plugin installation.").arg(ts3plugin_version()).arg(remoteVersion));
    //updateMsgBox.setDetailedText(QString("Local Version: %1\nRemote Version: %2").arg(ts3plugin_version()).arg(remoteVersion));
    updateMsgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    updateMsgBox.setDefaultButton(QMessageBox::Ok);
    updateMsgBox.setWindowTitle(ts3plugin_name());
    int ret = updateMsgBox.exec();    // blocking variant; might work to workaround api check disaster
//    updateMsgBox->open(this,SLOT(onButtonClicked(QAbstractButton*)));
//    updateMsgBox->deleteLater();

    // I must be missing something for the non-blocking variant to work -.-
    if (ret == QMessageBox::Ok || ret == QMessageBox::Yes)
    {
        QDesktopServices::openUrl((remoteVersion==m_VersionStable)?STABLE:BETA_DOWNLOAD);
        qApp->quit();
    }
    else if (ret == QMessageBox::Cancel || ret == QMessageBox::No)
        TSLogging::Log("Update Rejected",LogLevel_INFO);
    else
        TSLogging::Print("Could not find button role");
}

QUrl Updater::redirectUrl(const QUrl& possibleRedirectUrl, const QUrl& oldRedirectUrl) const
{
    QUrl redirectUrl;
    /*
     * Check if the URL is empty and
     * that we aren't being fooled into a infinite redirect loop.
     * We could also keep track of how many redirects we have been to
     * and set a limit to it, but we'll leave that to you.
     */
    if(!possibleRedirectUrl.isEmpty() && possibleRedirectUrl != oldRedirectUrl)
        redirectUrl = possibleRedirectUrl;

    return redirectUrl;
}

void Updater::CheckTriggerUpdateDialog()
{
//    TSLogging::Print(QString("Stable: %1").arg(m_VersionStable));
//    TSLogging::Print(QString("Beta: %1").arg(m_VersionBeta));
//    TSLogging::Print(QString("Local: %1").arg(ts3plugin_version()));

    if (!m_isBetaChannel)
    {
        if (m_VersionStable != ts3plugin_version())
            ShowUpdateDialog(m_VersionStable);
        else
            TSLogging::Log(QString("%1 version %2 is up to date.").arg(ts3plugin_name()).arg(ts3plugin_version()));
    }
    else
    {
        // Is Stable newer than beta?
        bool isStableNewer = false;
        if (m_VersionStable.isEmpty() && m_VersionBeta.isEmpty())
        {
            TSLogging::Log(QString("(Updater) Could not check for updates."));
        }
        else if (m_VersionStable.isEmpty()) // TS site is down
        {
            TSLogging::Log("(Updater) Could not get stable version info. Aborting.");
        }
        else if (m_VersionBeta.isEmpty()) // Beta site is down
        {
            TSLogging::Log("(Updater) Could not get beta version info.");
            isStableNewer = true;
        }
        else if (m_VersionBeta == m_VersionStable)
            isStableNewer = true;
        else
        {
            QStringList stable = m_VersionStable.split(".",QString::SkipEmptyParts);
            QStringList beta = m_VersionBeta.split(".",QString::SkipEmptyParts);
            bool isOk;
            for (int i = 0; i< stable.length(); ++i)
            {
                int stableInt = stable[i].toInt(&isOk,10);
                if (!isOk)
                    break;

                int betaInt = beta[i].toInt(&isOk,10);
                if (!isOk)
                    break;

                if (stableInt > betaInt)
                {
                    isStableNewer = true;
                    break;
                }
            }
            if (!isOk)
            {
                TSLogging::Log("(Updater) Problem with version comparison.");
                isStableNewer = true;
            }
//            TSLogging::Print(QString("(Updater) Remote Stable is %1 than beta.").arg((isStableNewer)?"newer":"older"));
        }

        if (isStableNewer && (m_VersionStable != ts3plugin_version()))
            ShowUpdateDialog(m_VersionStable);
        else if (!isStableNewer && (m_VersionBeta != ts3plugin_version()))
            ShowUpdateDialog(m_VersionBeta);
        else
            TSLogging::Log(QString("%1 version %2 is up to date.").arg(ts3plugin_name()).arg(ts3plugin_version()));
    }
}
