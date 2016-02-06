#include "config_radio_groupbox.h"
#include "ui_config_radio_groupbox.h"

ConfigRadioGroupBox::ConfigRadioGroupBox(QWidget *parent) :
    QGroupBox(parent),
    ui(new Ui::ConfigRadioGroupBox)
{
    ui->setupUi(this);

    connect(ui->dial_in_lo, &QDial::valueChanged, this, &ConfigRadioGroupBox::onInLoDialValueChanged);
    connect(ui->dial_in_hi, &QDial::valueChanged, this, &ConfigRadioGroupBox::onInHiDialValueChanged);
    connect(ui->dial_out_lo, &QDial::valueChanged, this, &ConfigRadioGroupBox::onOutLoDialValueChanged);
    connect(ui->dial_out_hi, &QDial::valueChanged, this, &ConfigRadioGroupBox::onOutHiDialValueChanged);
    connect(ui->dial_destr, &QDial::valueChanged, this, &ConfigRadioGroupBox::onDestrDialValueChanged);
    connect(ui->dial_rm, &QDial::valueChanged, this, &ConfigRadioGroupBox::onRingModFrequencyDialValueChanged);
    connect(ui->dial_rm_mix, &QDial::valueChanged, this, &ConfigRadioGroupBox::onRingModMixDialValueChanged);

    // QT5 on overloaded functions ain't pretty
    connect(ui->doubleSpinBox_in_lo, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &ConfigRadioGroupBox::onInLoValueChanged);
    connect(ui->doubleSpinBox_in_hi, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &ConfigRadioGroupBox::onInHiValueChanged);
    connect(ui->doubleSpinBox_out_lo, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &ConfigRadioGroupBox::onOutLoValueChanged);
    connect(ui->doubleSpinBox_out_hi,static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &ConfigRadioGroupBox::onOutHiValueChanged);
    connect(ui->doubleSpinBox_destr, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &ConfigRadioGroupBox::onDestrValueChanged);
    connect(ui->doubleSpinBox_rm, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &ConfigRadioGroupBox::onRingModFrequencyValueChanged);
    connect(ui->doubleSpinBox_rm_mix, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &ConfigRadioGroupBox::onRingModMixValueChanged);

    connect(this, &ConfigRadioGroupBox::toggled, this, &ConfigRadioGroupBox::onToggled);
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
