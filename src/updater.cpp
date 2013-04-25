#include "updater.h"

//#include <QDialog>

#include <QApplication>
#include <QDesktopServices>

#include "ts_helpers_qt.h"
#include "ts_logging_qt.h"

const QUrl STABLE("http://addons.teamspeak.com/directory/plugins/miscellaneous/CrossTalk.html");
const QUrl BETA_CHECK("http://dl.dropbox.com/u/18413693/CrossTalk-builds/package.ini");
const QUrl BETA_DOWNLOAD("http://jianji.de/crosstalk/");

//! Constructor
/*!
 * \brief Config::Config create an instance of this class
 */
Updater::Updater(QObject *parent) :
    QObject(parent)
{
    this->setObjectName("Updater");
    m_netwManager = new QNetworkAccessManager(this);
}

//! Retrieves update url
/*!
 * \brief Config::onNetwManagerFinished Qt slot to retrieve the content
 * \param reply a QNetworkReply containing the update data to parse
 */
void Updater::onNetwManagerFinished(QNetworkReply *reply)
{
    if (reply->error() != QNetworkReply::NoError) {
        TSLogging::Log(reply->errorString(),LogLevel_WARNING);
        return;
    }

    QByteArray arr = reply->readAll();
    reply->deleteLater();

    int start = arr.indexOf("Version",0);
    if (start == -1)
    {
        if (0 == arr.indexOf("302",0))
        {
            TSLogging::Log("Update check forwarding to...",LogLevel_INFO);
            start = arr.indexOf("http://",2);
            if (start == -1)
            {
                TSLogging::Log("Could not find forwarding address start.",LogLevel_WARNING);
                return;
            }
            int end = arr.indexOf(";",start);
            if (end == -1)
            {
                TSLogging::Log("Could not find forwarding address end.",LogLevel_WARNING);
                return;
            }
            arr = arr.mid(start,end-start);
            TSLogging::Log(arr, LogLevel_INFO);
            CheckUpdate(QString(arr));
        }
        else
            TSLogging::Log((this->objectName() + ": Did not find Version."),LogLevel_WARNING);

        return;
    }

    QString endStr = (checkUrl==STABLE)?ts3plugin_name():"Platforms";
    int end = arr.indexOf(endStr,start);
    if (end == -1)
    {
        TSLogging::Log((this->objectName() + ": Did not find %1.").arg(endStr),LogLevel_WARNING);
        return;
    }

    QString parse(arr.mid(start,end-start));
    QRegExp rx("\\d+(?:\\.\\d+)+");

    int pos = rx.indexIn(parse);
    if (pos > -1)
        parse = rx.cap(0);

    if (parse != ts3plugin_version())
        ShowUpdateDialog(parse);
    else
        TSLogging::Log(QString("%1 version %2 is up to date.").arg(ts3plugin_name()).arg(ts3plugin_version()));

    m_netwManager->deleteLater();
}

void Updater::CheckUpdate()
{
    QSettings cfg(TSHelpers::GetFullConfigPath(), QSettings::IniFormat);
    CheckUpdate((cfg.value("beta",false).toBool())?BETA_CHECK:STABLE);
}

void Updater::CheckUpdate(QUrl url)
{
    checkUrl = url;
    connect(m_netwManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(onNetwManagerFinished(QNetworkReply*)),Qt::UniqueConnection);
    QNetworkRequest request(checkUrl);
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
    // Get MainWindow
    QList<QWidget*> candidates;
    foreach (QWidget *widget, QApplication::topLevelWidgets()) {
        if (widget->isWindow() && !widget->windowTitle().isEmpty())
            candidates.append(widget);
    }
    QWidget* mainWindow;
    if (candidates.count() == 1)
        mainWindow = candidates.at(0);
    else
    {
        TSLogging::Error("Too many candidates for main window.");
    }

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
        QDesktopServices::openUrl((checkUrl==STABLE)?STABLE:BETA_DOWNLOAD);
        qApp->quit();
    }
    else if (ret == QMessageBox::Cancel || ret == QMessageBox::No)
        TSLogging::Print("Rejected");
    else
        TSLogging::Print("Could not find button role");
}
