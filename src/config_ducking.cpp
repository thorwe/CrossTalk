#include "config_ducking.h"
#include "ui_config_ducking.h"

//#include <QSettings>
//#include "ts_helpers_qt.h"

ConfigDucking::ConfigDucking(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConfigDucking)
{
    ui->setupUi(this);
    this->setAttribute( Qt::WA_DeleteOnClose );

    this->connect(ui->groupBox_gDuck,SIGNAL(toggled(bool)),SIGNAL(globalDuckerEnabledSet(bool)));
    this->connect(ui->groupBox_cDuck,SIGNAL(toggled(bool)),SIGNAL(channelDuckerEnabledSet(bool)));
    this->connect(ui->wFader_gDuck,SIGNAL(valueChanged(float)),SIGNAL(globalDuckerValueChanged(float)));
    this->connect(ui->wFader_cDuck,SIGNAL(valueChanged(float)),SIGNAL(channelDuckerValueChanged(float)));
    this->connect(ui->slider_cDuck_Mode,SIGNAL(valueChanged(int)),SLOT(onChannelDuckingDuckModeSliderValueChanged(int)));
}

ConfigDucking::~ConfigDucking()
{
    delete ui;
}

void ConfigDucking::UpdateGlobalDuckerEnabled(bool val)
{
    this->blockSignals(true);
    ui->groupBox_gDuck->setChecked(val);
    this->blockSignals(false);
}

void ConfigDucking::UpdateGlobalDuckerValue(float val)
{
    this->blockSignals(true);
    ui->wFader_gDuck->setValue(val);
    this->blockSignals(false);
}

void ConfigDucking::UpdateChannelDuckerEnabled(bool val)
{
    this->blockSignals(true);
    ui->groupBox_cDuck->setChecked(val);
    this->blockSignals(false);
}

void ConfigDucking::UpdateChannelDuckerValue(float val)
{
    this->blockSignals(true);
    ui->wFader_cDuck->setValue(val);
    this->blockSignals(false);
}

void ConfigDucking::UpdateChannelDuckerReverse(bool val)
{
    this->blockSignals(true);
    ui->slider_cDuck_Mode->setValue(val);
    onChannelDuckingDuckModeSliderValueChanged(val);
    this->blockSignals(false);
}

//! Retrieves ducking mode slider change, sets label, emits channelDuckerReverseSet as bool
/*!
 * \brief Config::onChannelDuckingDuckModeSliderValueChanged Retrieves ducking mode slider change, sets label, emits channelDuckerReverseSet as bool
 * \param val slider value (0-100)
 */
void ConfigDucking::onChannelDuckingDuckModeSliderValueChanged(int val)
{
    bool isReversed = (val==1);
    if (isReversed)
    {
        QString otherServers = qApp->translate("NotificationsSetup","Other");
        otherServers.append(" ");
        otherServers.append(qApp->translate("ImprovedTabBar", "Server Tabs"));
        ui->label_cDuck_Target_Dyn->setText(otherServers);
    }
    else
        ui->label_cDuck_Target_Dyn->setText(qApp->translate("HotkeyDialog","Current Server"));

    emit channelDuckerReverseSet(isReversed);
}
