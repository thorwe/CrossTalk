#include "banner_frame.h"
#include "ui_banner_frame.h"

#include <QtGui/QDesktopServices>

#include "core/ts_helpers_qt.h"
#include "core/ts_logging_qt.h"

const QUrl CROSSTALK_CAMPAING("http://thorwe.github.io/CrossTalk/");
const QUrl JIANJI_CAMPAIGN(QStringLiteral("http://www.jianji.de"));

BannerFrame::BannerFrame(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::BannerFrame)
{
    ui->setupUi(this);

    // load image
    connect(ui->banner_logo, &Banner::onClick, this, &BannerFrame::onCrossTalkClicked);

    // load image
    connect(ui->banner_jianji, &Banner::onClick, this, &BannerFrame::onJianjiClicked);

    auto lang = TSHelpers::GetLanguage();
    if (lang != QStringLiteral("de_DE")) // outside of german locale, hide jianji banner
        ui->banner_jianji->setVisible(false);
}

BannerFrame::~BannerFrame()
{
    delete ui;
}

//! Receives the click of the crosstalk banner
/*!
 * \brief BannerFrame::onCrossTalkClicked Qt slot to receive the click of the crosstalk banner
 */
void BannerFrame::onCrossTalkClicked()
{
    QDesktopServices::openUrl(CROSSTALK_CAMPAING);
}

//! Receives the click of the jianji banner
/*!
* \brief Config::onJianjiClicked Qt slot to receive the click of the jianji banner
*/
void BannerFrame::onJianjiClicked()
{
    QUrl url(JIANJI_CAMPAIGN);
    QDesktopServices::openUrl(url);
}