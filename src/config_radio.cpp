#include "config_radio.h"
#include "ui_config_radio.h"

#include "mod_radio.h"
#include "config_radio_groupbox.h"
#include "config_radio_channelstrip.h"

#include "ts_helpers_qt.h"

ConfigRadio::ConfigRadio(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConfigRadio)
{
    ui->setupUi(this);

    this->setAttribute( Qt::WA_DeleteOnClose );

    connect(ui->groupBox_Home,SIGNAL(toggled(bool)),this,SIGNAL(HomeEnabledSet(bool)));
    connect(ui->groupBox_Home,SIGNAL(CenterFrequencySet(double)),this,SIGNAL(HomeLowFrequencySet(double)));
    connect(ui->groupBox_Home,SIGNAL(BandWidthSet(double)),this,SIGNAL(HomeHighFrequencySet(double)));
    connect(ui->groupBox_Home,SIGNAL(DestructionSet(double)),this,SIGNAL(HomeDestructionSet(double)));

    connect(ui->groupBox_Whisper,SIGNAL(toggled(bool)),this,SIGNAL(WhisperEnabledSet(bool)));
    connect(ui->groupBox_Whisper,SIGNAL(CenterFrequencySet(double)),this,SIGNAL(WhisperLowFrequencySet(double)));
    connect(ui->groupBox_Whisper,SIGNAL(BandWidthSet(double)),this,SIGNAL(WhisperHighFrequencySet(double)));
    connect(ui->groupBox_Whisper,SIGNAL(DestructionSet(double)),this,SIGNAL(WhisperDestructionSet(double)));

    connect(ui->groupBox_Other,SIGNAL(toggled(bool)),this,SIGNAL(OtherEnabledSet(bool)));
    connect(ui->groupBox_Other,SIGNAL(CenterFrequencySet(double)),this,SIGNAL(OtherLowFrequencySet(double)));
    connect(ui->groupBox_Other,SIGNAL(BandWidthSet(double)),this,SIGNAL(OtherHighFrequencySet(double)));
    connect(ui->groupBox_Other,SIGNAL(DestructionSet(double)),this,SIGNAL(OtherDestructionSet(double)));
}

ConfigRadio::~ConfigRadio()
{
    delete ui;
}

void ConfigRadio::UpdateHomeEnabled(bool val)
{
    ui->groupBox_Home->blockSignals(true);
    ui->groupBox_Home->setChecked(val);
    ui->groupBox_Home->blockSignals(false);
}

void ConfigRadio::UpdateHomeLowFrequency(double val)
{
    ui->groupBox_Home->blockSignals(true);
    ui->groupBox_Home->onCFValueChanged(val);
    ui->groupBox_Home->blockSignals(false);
}

void ConfigRadio::UpdateHomeHighFrequency(double val)
{
    ui->groupBox_Home->blockSignals(true);
    ui->groupBox_Home->onBWValueChanged(val);
    ui->groupBox_Home->blockSignals(false);
}

void ConfigRadio::UpdateHomeDestruction(double val)
{
    ui->groupBox_Home->blockSignals(true);
    ui->groupBox_Home->onDestrValueChanged(val);
    ui->groupBox_Home->blockSignals(false);
}

void ConfigRadio::UpdateWhisperEnabled(bool val)
{
    ui->groupBox_Whisper->blockSignals(true);
    ui->groupBox_Whisper->setChecked(val);
    ui->groupBox_Whisper->blockSignals(false);
}

void ConfigRadio::UpdateWhisperLowFrequency(double val)
{
    ui->groupBox_Whisper->blockSignals(true);
    ui->groupBox_Whisper->onCFValueChanged(val);
    ui->groupBox_Whisper->blockSignals(false);
}

void ConfigRadio::UpdateWhisperHighFrequency(double val)
{
    ui->groupBox_Whisper->blockSignals(true);
    ui->groupBox_Whisper->onBWValueChanged(val);
    ui->groupBox_Whisper->blockSignals(false);
}

void ConfigRadio::UpdateWhisperDestruction(double val)
{
    ui->groupBox_Whisper->blockSignals(true);
    ui->groupBox_Whisper->onDestrValueChanged(val);
    ui->groupBox_Whisper->blockSignals(false);
}

void ConfigRadio::UpdateOtherEnabled(bool val)
{
    ui->groupBox_Other->blockSignals(true);
    ui->groupBox_Other->setChecked(val);
    ui->groupBox_Other->blockSignals(false);
}

void ConfigRadio::UpdateOtherLowFrequency(double val)
{
    ui->groupBox_Other->blockSignals(true);
    ui->groupBox_Other->onCFValueChanged(val);
    ui->groupBox_Other->blockSignals(false);
}

void ConfigRadio::UpdateOtherHighFrequency(double val)
{
    ui->groupBox_Other->blockSignals(true);
    ui->groupBox_Other->onBWValueChanged(val);
    ui->groupBox_Other->blockSignals(false);
}

void ConfigRadio::UpdateOtherDestruction(double val)
{
    ui->groupBox_Other->blockSignals(true);
    ui->groupBox_Other->onDestrValueChanged(val);
    ui->groupBox_Other->blockSignals(false);
}
