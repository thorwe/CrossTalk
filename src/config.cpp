#include "config.h"
#include "ui_config.h"

#include <QtCore/QSettings>
#include <QWhatsThis>
#include <QDesktopServices>

#include "ts_logging_qt.h"
#include "ts_helpers_qt.h"

const QUrl JIANJI_CAMPAIGN("http://www.jianji.de");

//! Constructor
/*!
 * \brief Config::Config create an instance of this class
 */
Config::Config(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Config)
{
    ui->setupUi(this);
    this->setAttribute( Qt::WA_DeleteOnClose );
    this->setFixedSize(this->width(),this->height());

    // load image
    connect(ui->banner_jianji,SIGNAL(onClick()),SLOT(onJianjiClicked()));

    QString lang = TSHelpers::GetLanguage();
    if (lang != "de_DE") // outside of german locale, hide jianji banner
        ui->banner_jianji->setVisible(false);

    QSettings cfg(TSHelpers::GetFullConfigPath(), QSettings::IniFormat);
    ui->betaChannelcheckBox->setChecked(cfg.value("beta",false).toBool());
    connect(ui->betaChannelcheckBox,SIGNAL(toggled(bool)),SLOT(onBetaChannelToggled(bool)));

    bool ok;
    quint16 port = cfg.value("server_port",64736).toUInt(&ok);
    if (!ok)
    {
        TSLogging::Error("Could not read port from settings");
        ui->groupBox_Server->setChecked(false);
        ui->groupBox_Server->setCheckable(false);
        ui->spinBox_Server->setValue(0);
    }
    else
    {
        ui->spinBox_Server->setValue(port);
        connect(ui->spinBox_Server,SIGNAL(valueChanged(int)),this,SLOT(onServerPortChanged(int)));
        ui->groupBox_Server->setChecked(cfg.value("server_enabled",false).toBool());
        connect(ui->groupBox_Server,SIGNAL(toggled(bool)),this,SLOT(onServerEnabledToggled(bool)));
    }
}

//! Destructor
/*!
 * \brief Config::~Config Destroys the class instance
 */
Config::~Config()
{
    delete ui;
}

//! Receives the click of the jianji banner
/*!
 * \brief Config::onJianjiClicked Qt slot to receive the click of the jianji banner
 */
void Config::onJianjiClicked()
{
    QUrl url(JIANJI_CAMPAIGN);
    QDesktopServices::openUrl(url);
}

void Config::onBetaChannelToggled(bool val)
{
    QSettings cfg(TSHelpers::GetFullConfigPath(), QSettings::IniFormat);
    cfg.setValue("beta",val);
    emit betaChannelToggled(val);
}

void Config::onServerEnabledToggled(bool val)
{
    QSettings cfg(TSHelpers::GetFullConfigPath(), QSettings::IniFormat);
    cfg.setValue("server_enabled",val);
    emit serverEnabledToggled(val);
}

void Config::onServerPortChanged(int val)
{
    QSettings cfg(TSHelpers::GetFullConfigPath(), QSettings::IniFormat);
    cfg.setValue("server_port",(quint16)val);
    emit serverPortChanged((quint16)val);
}
