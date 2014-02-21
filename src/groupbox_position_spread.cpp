#include "groupbox_position_spread.h"
#include "ui_groupbox_position_spread.h"

GroupBoxPositionSpread::GroupBoxPositionSpread(QWidget *parent) :
    QGroupBox(parent),
    ui(new Ui::GroupBoxPositionSpread)
{
    ui->setupUi(this);

    ui->label_SPS_Home->setText(qApp->translate("HotkeyDialog","Current Server"));
    ui->label_SPS_Whisper->setText(qApp->translate("WhisperSetup","Whisper"));
    ui->label_SPS_Other->setText(qApp->translate("NotificationsSetup","Other"));
    ui->groupBox_SPS_expert->setTitle(qApp->translate("captureSoundSetupDialog","&Advanced Options"));

    connect(this,SIGNAL(toggled(bool)),this,SIGNAL(EnabledSet(bool))); // remove this redundancy later?
    connect(ui->groupBox_SPS_expert,SIGNAL(toggled(bool)),this,SIGNAL(ExpertEnabledSet(bool)));

    connect(ui->dial_SPS,SIGNAL(valueChanged(int)),this,SLOT(onDialValueChanged(int)));
    connect(ui->doubleSpinBox_SPS,SIGNAL(valueChanged(double)),this,SLOT(onSpinBoxValueChanged(double)));

    connect(ui->slider_SPS_Home,SIGNAL(valueChanged(int)),this,SIGNAL(ExpertHomeValueChanged(int)));
    connect(ui->slider_SPS_Whisper,SIGNAL(valueChanged(int)),this,SIGNAL(ExpertWhisperValueChanged(int)));
    connect(ui->slider_SPS_Other,SIGNAL(valueChanged(int)),this,SIGNAL(ExpertOtherValueChanged(int)));
}

GroupBoxPositionSpread::~GroupBoxPositionSpread()
{
    delete ui;
}

void GroupBoxPositionSpread::UpdateEnabledSet(bool val)
{
    this->blockSignals(true);
    this->setChecked(val);
    this->blockSignals(false);
}

void GroupBoxPositionSpread::UpdateSpreadWidth(float val)
{
    this->blockSignals(true);
    ui->doubleSpinBox_SPS->setValue(static_cast< double > (val*100));
    ui->dial_SPS->setValue(static_cast< int >(val*100));
    this->blockSignals(false);
}

void GroupBoxPositionSpread::UpdateExpertEnabledSet(bool val)
{
    this->blockSignals(true);
    ui->groupBox_SPS_expert->setChecked(val);
    this->blockSignals(false);
}

void GroupBoxPositionSpread::UpdateExpertHomeValueChanged(int val)
{
    this->blockSignals(true);
    ui->slider_SPS_Home->setValue(val);
    this->blockSignals(false);
}

void GroupBoxPositionSpread::UpdateExpertWhisperValueChanged(int val)
{
    this->blockSignals(true);
    ui->slider_SPS_Whisper->setValue(val);
    this->blockSignals(false);
}

void GroupBoxPositionSpread::UpdateExpertOtherValueChanged(int val)
{
    this->blockSignals(true);
    ui->slider_SPS_Other->setValue(val);
    this->blockSignals(false);
}

//! Retrieves SPS value changed from dial and forwards it to spinbox
/*!
 * \brief GroupBoxPositionSpread::onDialValueChanged Qt slot to retrieve SPS value changed from dial and forwards it to spinbox
 * \param val dial value
 */
void GroupBoxPositionSpread::onDialValueChanged(int val)
{
    ui->doubleSpinBox_SPS->setValue(static_cast< double > (val));
}

//! Retrieves SPS value changed from spinbox, sets slider if it's not the initial sender, emits SetStereoPositionSpreadValue
/*!
 * \brief GroupBoxPositionSpread::onSliderValueChanged Qt slot to retrieve SPS value changed from spinbox
 * \param val spinbox value
 */
void GroupBoxPositionSpread::onSpinBoxValueChanged(double val)
{
    if (!(ui->dial_SPS->isSliderDown()))                // loop breaker
        ui->dial_SPS->setValue(static_cast< int >(val));

    emit ValueChanged(static_cast< float >(val * 0.01));
}
