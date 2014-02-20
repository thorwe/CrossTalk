#include "config_radio_groupbox.h"
#include "ui_config_radio_groupbox.h"

ConfigRadioGroupBox::ConfigRadioGroupBox(QWidget *parent) :
    QGroupBox(parent),
    ui(new Ui::ConfigRadioGroupBox)
{
    ui->setupUi(this);

    connect(ui->dial_in_lo,SIGNAL(valueChanged(int)),this,SLOT(onInLoDialValueChanged(int)));
    connect(ui->dial_in_hi,SIGNAL(valueChanged(int)),this,SLOT(onInHiDialValueChanged(int)));
    connect(ui->dial_out_lo,SIGNAL(valueChanged(int)),this,SLOT(onOutLoDialValueChanged(int)));
    connect(ui->dial_out_hi,SIGNAL(valueChanged(int)),this,SLOT(onOutHiDialValueChanged(int)));
    connect(ui->dial_destr,SIGNAL(valueChanged(int)),this,SLOT(onDestrDialValueChanged(int)));
    connect(ui->dial_rm,SIGNAL(valueChanged(int)),this,SLOT(onRingModFrequencyDialValueChanged(int)));
    connect(ui->dial_rm_mix,SIGNAL(valueChanged(int)),this,SLOT(onRingModMixDialValueChanged(int)));

    connect(ui->doubleSpinBox_in_lo,SIGNAL(valueChanged(double)),this,SLOT(onInLoValueChanged(double)));
    connect(ui->doubleSpinBox_in_hi,SIGNAL(valueChanged(double)),this,SLOT(onInHiValueChanged(double)));
    connect(ui->doubleSpinBox_out_lo,SIGNAL(valueChanged(double)),this,SLOT(onOutLoValueChanged(double)));
    connect(ui->doubleSpinBox_out_hi,SIGNAL(valueChanged(double)),this,SLOT(onOutHiValueChanged(double)));
    connect(ui->doubleSpinBox_destr,SIGNAL(valueChanged(double)),this,SLOT(onDestrValueChanged(double)));
    connect(ui->doubleSpinBox_rm,SIGNAL(valueChanged(double)),this,SLOT(onRingModFrequencyValueChanged(double)));
    connect(ui->doubleSpinBox_rm_mix,SIGNAL(valueChanged(double)),this,SLOT(onRingModMixValueChanged(double)));

    connect(this,SIGNAL(toggled(bool)),this,SLOT(onToggled(bool)));
}

ConfigRadioGroupBox::~ConfigRadioGroupBox()
{
    delete ui;
}

void ConfigRadioGroupBox::onToggled(bool val)
{
    emit EnabledSet(this->objectName(),val);
}

void ConfigRadioGroupBox::onInLoValueChanged(double val)
{
    if (!(ui->dial_in_lo->isSliderDown()))                // loop breaker
        ui->dial_in_lo->setValue(static_cast< int >(val));

    emit InLoFreqSet(this->objectName(),val);
}

void ConfigRadioGroupBox::onInHiValueChanged(double val)
{
    if (!(ui->dial_in_hi->isSliderDown()))                // loop breaker
        ui->dial_in_hi->setValue(static_cast< int >(val));

    emit InHiFreqSet(this->objectName(),val);
}

void ConfigRadioGroupBox::onDestrValueChanged(double val)
{
    if (!(ui->dial_destr->isSliderDown()))                // loop breaker
        ui->dial_destr->setValue(static_cast< int >(val));

    emit DestructionSet(this->objectName(),val);
}

void ConfigRadioGroupBox::onRingModFrequencyValueChanged(double val)
{
    if (!(ui->dial_rm->isSliderDown()))                 // loop breaker
        ui->dial_rm->setValue(static_cast< int >(val));

    emit RingModFrequencySet(this->objectName(),val);
}

void ConfigRadioGroupBox::onRingModMixValueChanged(double val)
{
    if (!(ui->dial_rm_mix->isSliderDown()))                 // loop breaker
        ui->dial_rm_mix->setValue(static_cast< int >(val * 100));

    emit RingModMixSet(this->objectName(),val);
}

void ConfigRadioGroupBox::onOutLoValueChanged(double val)
{
    if (!(ui->dial_out_lo->isSliderDown()))                // loop breaker
        ui->dial_out_lo->setValue(static_cast< int >(val));

    emit OutLoFreqSet(this->objectName(),val);
}

void ConfigRadioGroupBox::onOutHiValueChanged(double val)
{
    if (!(ui->dial_out_hi->isSliderDown()))                // loop breaker
        ui->dial_out_hi->setValue(static_cast< int >(val));

    emit OutHiFreqSet(this->objectName(),val);
}

// private

void ConfigRadioGroupBox::onInLoDialValueChanged(int val)
{
    ui->doubleSpinBox_in_lo->setValue(static_cast< double > (val));
}

void ConfigRadioGroupBox::onInHiDialValueChanged(int val)
{
    ui->doubleSpinBox_in_hi->setValue(static_cast< double > (val));
}

void ConfigRadioGroupBox::onOutLoDialValueChanged(int val)
{
    ui->doubleSpinBox_out_lo->setValue(static_cast< double > (val));
}

void ConfigRadioGroupBox::onOutHiDialValueChanged(int val)
{
    ui->doubleSpinBox_out_hi->setValue(static_cast< double > (val));
}

void ConfigRadioGroupBox::onDestrDialValueChanged(int val)
{
    ui->doubleSpinBox_destr->setValue(static_cast< double > (val));
}

void ConfigRadioGroupBox::onRingModFrequencyDialValueChanged(int val)
{
    ui->doubleSpinBox_rm->setValue(static_cast< double > (val));
}

void ConfigRadioGroupBox::onRingModMixDialValueChanged(int val)
{
    ui->doubleSpinBox_rm_mix->setValue((static_cast< double > (val)) / 100.0f);
}
