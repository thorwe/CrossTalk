#include "config.h"
#include "ui_config.h"

#include <QtCore/QSettings>
#include <QtGui/QDesktopServices>
#include <QtWidgets/QLabel>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QCheckBox>

#include "core/ts_logging_qt.h"
#include "core/ts_helpers_qt.h"



//! Constructor
/*!
 * \brief Config::Config create an instance of this class
 */
Config::Config(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Config)
{
    ui->setupUi(this);
    setAttribute( Qt::WA_DeleteOnClose );
    setFixedSize(width(), height());

    // Create Settings UI

    auto groupBoxWSS = new QGroupBox(tr("WebSockets Server"), this);
    groupBoxWSS->setCheckable(true);
    auto groupBoxWSSLayout = new QHBoxLayout;
    auto wssLabel = new QLabel(tr("Port"), this);
    groupBoxWSSLayout->addWidget(wssLabel, 0, Qt::AlignRight);
    auto wssSpinBox = new QSpinBox(this);
    wssSpinBox->setMaximum(65535);
    wssSpinBox->setMinimumWidth(80);
    groupBoxWSSLayout->addWidget(wssSpinBox, 0, Qt::AlignLeft);
    groupBoxWSS->setLayout(groupBoxWSSLayout);
    ui->horizontalLayout_Settings->addWidget(groupBoxWSS);


    // Fill with settings
    QSettings cfg(TSHelpers::GetFullConfigPath(), QSettings::IniFormat);

    // WebSocket Server
    bool ok;
    quint16 port = cfg.value("server_port", 64734).toUInt(&ok);
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
        connect(wssSpinBox, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this,  &Config::onServerPortChanged);
        groupBoxWSS->setChecked(cfg.value("server_enabled", true).toBool());
        connect(groupBoxWSS, &QGroupBox::toggled, this, &Config::onServerEnabledToggled);
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
