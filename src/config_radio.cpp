#include "config_radio.h"
#include "ui_config_radio.h"

#include "mod_radio.h"
#include "ts_helpers_qt.h"

#include "ts_logging_qt.h"

ConfigRadio::ConfigRadio(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConfigRadio)
{
    ui->setupUi(this);
    this->setAttribute( Qt::WA_DeleteOnClose );
    this->setFixedSize(this->width(),this->height());
}

ConfigRadio::~ConfigRadio()
{
    delete ui;
}

void ConfigRadio::UpdateEnabled(QString name, bool val)
{
    ConfigRadioGroupBox* channelStrip = GetChannelStrip(name);
    channelStrip->blockSignals(true);
    channelStrip->setChecked(val);
    channelStrip->blockSignals(false);
}

void ConfigRadio::UpdateBandpassInLowFrequency(QString name, double val)
{
    ConfigRadioGroupBox* channelStrip = GetChannelStrip(name);
    channelStrip->blockSignals(true);
    channelStrip->onInLoValueChanged(val);
    channelStrip->blockSignals(false);
}

void ConfigRadio::UpdateBandpassInHighFrequency(QString name, double val)
{
    ConfigRadioGroupBox* channelStrip = GetChannelStrip(name);
    channelStrip->blockSignals(true);
    channelStrip->onInHiValueChanged(val);
    channelStrip->blockSignals(false);
}

void ConfigRadio::UpdateDestruction(QString name, double val)
{
    ConfigRadioGroupBox* channelStrip = GetChannelStrip(name);
    channelStrip->blockSignals(true);
    channelStrip->onDestrValueChanged(val);
    channelStrip->blockSignals(false);
}

void ConfigRadio::UpdateRingModFrequency(QString name, double val)
{
    ConfigRadioGroupBox* channelStrip = GetChannelStrip(name);
    channelStrip->blockSignals(true);
    channelStrip->onRingModFrequencyValueChanged(val);
    channelStrip->blockSignals(false);
}

void ConfigRadio::UpdateRingModMix(QString name, double val)
{
    ConfigRadioGroupBox* channelStrip = GetChannelStrip(name);
    channelStrip->blockSignals(true);
    channelStrip->onRingModMixValueChanged(val);
    channelStrip->blockSignals(false);
}

void ConfigRadio::UpdateBandpassOutLowFrequency(QString name, double val)
{
    ConfigRadioGroupBox* channelStrip = GetChannelStrip(name);
    channelStrip->blockSignals(true);
    channelStrip->onOutLoValueChanged(val);
    channelStrip->blockSignals(false);
}

void ConfigRadio::UpdateBandpassOutHighFrequency(QString name, double val)
{
    ConfigRadioGroupBox* channelStrip = GetChannelStrip(name);
    channelStrip->blockSignals(true);
    channelStrip->onOutHiValueChanged(val);
    channelStrip->blockSignals(false);
}


ConfigRadioGroupBox* ConfigRadio::GetChannelStrip(QString name)
{
    if (m_ChannelStripMap.contains(name))
        return m_ChannelStripMap.value(name);

    ConfigRadioGroupBox* channelStrip = new ConfigRadioGroupBox(this);
    channelStrip->setObjectName(name);
    channelStrip->setTitle(name);
    connect(channelStrip,SIGNAL(EnabledSet(QString,bool)),this,SIGNAL(EnabledSet(QString,bool)));
    connect(channelStrip,SIGNAL(InLoFreqSet(QString,double)),this,SIGNAL(InLoFreqSet(QString,double)));
    connect(channelStrip,SIGNAL(InHiFreqSet(QString,double)),this,SIGNAL(InHiFreqSet(QString,double)));
    connect(channelStrip,SIGNAL(DestructionSet(QString,double)),this,SIGNAL(DestructionSet(QString,double)));
    connect(channelStrip,SIGNAL(RingModFrequencySet(QString,double)),this,SIGNAL(RingModFrequencySet(QString,double)));
    connect(channelStrip,SIGNAL(RingModMixSet(QString,double)),this,SIGNAL(RingModMixSet(QString,double)));
    connect(channelStrip,SIGNAL(OutLoFreqSet(QString,double)),this,SIGNAL(OutLoFreqSet(QString,double)));
    connect(channelStrip,SIGNAL(OutHiFreqSet(QString,double)),this,SIGNAL(OutHiFreqSet(QString,double)));
    ui->horizontalLayout->addWidget(channelStrip);
    m_ChannelStripMap.insert(name,channelStrip);
    //TSLogging::Print(QString("Channelstrip created: %1").arg(name));

    return channelStrip;
}
