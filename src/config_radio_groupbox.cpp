#include "config_radio_groupbox.h"
#include "ui_config_radio_groupbox.h"

ConfigRadioGroupBox::ConfigRadioGroupBox(QWidget *parent) :
    QGroupBox(parent),
    ui(new Ui::ConfigRadioGroupBox)
{
    ui->setupUi(this);

    connect(ui->dial_cf,SIGNAL(valueChanged(int)),this,SLOT(onCFDialValueChanged(int)));
    connect(ui->dial_bw,SIGNAL(valueChanged(int)),this,SLOT(onBWDialValueChanged(int)));
    connect(ui->dial_destr,SIGNAL(valueChanged(int)),this,SLOT(onDestrDialValueChanged(int)));
    connect(ui->dial_rm,SIGNAL(valueChanged(int)),this,SLOT(onRingModFrequencyDialValueChanged(int)));

    connect(ui->doubleSpinBox_cf,SIGNAL(valueChanged(double)),this,SLOT(onCFValueChanged(double)));
    connect(ui->doubleSpinBox_bw,SIGNAL(valueChanged(double)),this,SLOT(onBWValueChanged(double)));
    connect(ui->doubleSpinBox_destr,SIGNAL(valueChanged(double)),this,SLOT(onDestrValueChanged(double)));
    connect(ui->doubleSpinBox_rm,SIGNAL(valueChanged(double)),this,SLOT(onRingModFrequencyValueChanged(double)));

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

void ConfigRadioGroupBox::onCFValueChanged(double val)
{
    if (!(ui->dial_cf->isSliderDown()))                // loop breaker
        ui->dial_cf->setValue(static_cast< int >(val));

    emit CenterFrequencySet(this->objectName(),val);
}

void ConfigRadioGroupBox::onBWValueChanged(double val)
{
    if (!(ui->dial_bw->isSliderDown()))                // loop breaker
        ui->dial_bw->setValue(static_cast< int >(val));

    emit BandWidthSet(this->objectName(),val);
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

// private

void ConfigRadioGroupBox::onCFDialValueChanged(int val)
{
    ui->doubleSpinBox_cf->setValue(static_cast< double > (val));
}

void ConfigRadioGroupBox::onBWDialValueChanged(int val)
{
    ui->doubleSpinBox_bw->setValue(static_cast< double > (val));
}

void ConfigRadioGroupBox::onDestrDialValueChanged(int val)
{
    ui->doubleSpinBox_destr->setValue(static_cast< double > (val));
}

void ConfigRadioGroupBox::onRingModFrequencyDialValueChanged(int val)
{
    ui->doubleSpinBox_rm->setValue(static_cast< double > (val));
}
