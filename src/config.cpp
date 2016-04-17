#include "config.h"
#include "ui_config.h"

#include <QtCore/QSettings>
#include <QWhatsThis>
#include <QDesktopServices>
#include <QHBoxLayout>
#include <QLabel>
#include <QSpinBox>

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
    connect(ui->banner_jianji, &Banner::onClick, this, &Config::onJianjiClicked);

    auto lang = TSHelpers::GetLanguage();
    if (lang != "de_DE") // outside of german locale, hide jianji banner
        ui->banner_jianji->setVisible(false);

    // Create Settings UI
    auto groupBoxBeta = new QGroupBox(tr("Plugin Updates"),this);
    auto groupBoxBetaLayout = new QHBoxLayout;
    auto checkBoxBeta = new QCheckBox(tr("Beta Channel"),this);
    groupBoxBetaLayout->addWidget(checkBoxBeta);
    groupBoxBeta->setLayout(groupBoxBetaLayout);

    auto groupBoxWSS = new QGroupBox(tr("WebSockets Server"),this);
    groupBoxWSS->setCheckable(true);
    auto groupBoxWSSLayout = new QHBoxLayout;
    auto wssLabel = new QLabel(tr("Port"),this);
    groupBoxWSSLayout->addWidget(wssLabel,0,Qt::AlignRight);
    auto wssSpinBox = new QSpinBox(this);
    wssSpinBox->setMaximum(65535);
    wssSpinBox->setMinimumWidth(80);
    groupBoxWSSLayout->addWidget(wssSpinBox,0,Qt::AlignLeft);
    groupBoxWSS->setLayout(groupBoxWSSLayout);

    auto groupBoxSSE = new QGroupBox(tr("Local SSE-Server"),this);
    groupBoxSSE->setCheckable(true);
    auto groupBoxSSELayout = new QHBoxLayout;
    auto sseLabel = new QLabel(tr("Port"),this);
    groupBoxSSELayout->addWidget(sseLabel,0,Qt::AlignRight);
    auto sseSpinBox = new QSpinBox(this);
    sseSpinBox->setMaximum(65535);
    sseSpinBox->setMinimumWidth(80);
    groupBoxSSELayout->addWidget(sseSpinBox,0,Qt::AlignLeft);
    groupBoxSSE->setLayout(groupBoxSSELayout);

    ui->horizontalLayout_Settings->addWidget(groupBoxBeta);
    ui->horizontalLayout_Settings->addWidget(groupBoxWSS);
    ui->horizontalLayout_Settings->addWidget(groupBoxSSE);

    // Fill with settings
    QSettings cfg(TSHelpers::GetFullConfigPath(), QSettings::IniFormat);

    checkBoxBeta->setChecked(cfg.value("beta",true).toBool());
    connect(checkBoxBeta, &QCheckBox::toggled, this, &Config::onBetaChannelToggled);

    // WebSocket Server
    bool ok;
    quint16 port = cfg.value("server_port",64734).toUInt(&ok);
    if (!ok)
    {
        TSLogging::Error("Could not read websocket server port from settings");
        groupBoxWSS->setChecked(false);
        groupBoxWSS->setCheckable(false);
        wssSpinBox->setValue(0);
    }
    else
    {
        wssSpinBox->setValue(port);
        connect(wssSpinBox, SIGNAL(valueChanged(int)),this,SLOT(onServerPortChanged(int)));
        groupBoxWSS->setChecked(cfg.value("server_enabled",true).toBool());
        connect(groupBoxWSS, &QGroupBox::toggled, this, &Config::onServerEnabledToggled);
    }
    // SSE-Server
    port = cfg.value("sse_server_port",64736).toUInt(&ok);
    if (!ok)
    {
        TSLogging::Error("Could not read sse server port from settings");
        groupBoxSSE->setChecked(false);
        groupBoxSSE->setCheckable(false);
        sseSpinBox->setValue(0);
    }
    else
    {
        sseSpinBox->setValue(port);
        connect(sseSpinBox,SIGNAL(valueChanged(int)),this,SLOT(onSseServerPortChanged(int)));
        groupBoxSSE->setChecked(cfg.value("sse_server_enabled",false).toBool());
        connect(groupBoxSSE, &QGroupBox::toggled, this, &Config::onSseServerEnabledToggled);
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

void Config::onSseServerEnabledToggled(bool val)
{
    QSettings cfg(TSHelpers::GetFullConfigPath(), QSettings::IniFormat);
    cfg.setValue("sse_server_enabled",val);
    emit sseServerEnabledToggled(val);
}

void Config::onSseServerPortChanged(int val)
{
    QSettings cfg(TSHelpers::GetFullConfigPath(), QSettings::IniFormat);
    cfg.setValue("sse_server_port",(quint16)val);
    emit sseServerPortChanged((quint16)val);
}
