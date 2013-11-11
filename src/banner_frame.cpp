#include "banner_frame.h"
#include "ui_banner_frame.h"

#include <QDesktopServices>

#include "ts_logging_qt.h"

const QUrl PLEDGIE_IMAGE("http://pledgie.com/campaigns/18898.png");
const QUrl PLEDGIE_CAMPAIGN("http://www.pledgie.com/campaigns/18898");
const QUrl CROSSTALK_CAMPAING("http://addons.teamspeak.com/directory/plugins/miscellaneous/CrossTalk.html");

BannerFrame::BannerFrame(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::BannerFrame)
{
    ui->setupUi(this);
    m_netwManager = new QNetworkAccessManager(this);

    // load image
    connect(ui->banner_pledgie,SIGNAL(onClick()),this,SLOT(onPledgieClicked()));
    connect(ui->banner_logo,SIGNAL(onClick()),this,SLOT(onCrossTalkClicked()));

    connect(m_netwManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(onNetwManagerFinished(QNetworkReply*)));
    QNetworkRequest request(PLEDGIE_IMAGE);
    m_netwManager->get(request);
}

BannerFrame::~BannerFrame()
{
    delete ui;
}

void BannerFrame::onNetwManagerFinished(QNetworkReply *reply)
{
    if (reply->error() != QNetworkReply::NoError)
    {
        TSLogging::Log(reply->errorString(),LogLevel_WARNING);
        return;
    }

    QByteArray jpegData = reply->readAll();
    QPixmap pixmap;
    if (!(pixmap.loadFromData(jpegData)))
    {
        // until I figure sth. out regarding the image (https not supported issue) suppress error
        //TSLogging::Log("Error loading pledgie image.",LogLevel_WARNING);
    }
    else if ((reply->url()) == PLEDGIE_IMAGE)
        ui->banner_pledgie->setPixmap(pixmap);

    reply->deleteLater();
}

//! Receives the click of the pledgie banner
/*!
 * \brief BannerFrame::onPledgieClicked Qt slot to receive the click of the pledgie banner
 */
void BannerFrame::onPledgieClicked()
{
    QDesktopServices::openUrl(PLEDGIE_CAMPAIGN);
}

//! Receives the click of the crosstalk banner
/*!
 * \brief BannerFrame::onCrossTalkClicked Qt slot to receive the click of the crosstalk banner
 */
void BannerFrame::onCrossTalkClicked()
{
    QDesktopServices::openUrl(CROSSTALK_CAMPAING);
}
