#include "fader_vertical.h"
#include "ui_fader_vertical.h"

#include "MMtoDB.h"

FaderVertical::FaderVertical(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FaderVertical)
{
    ui->setupUi(this);
    this->connect(ui->horizontalSlider,SIGNAL(valueChanged(int)),SLOT(onSliderValueChanged(int)));
    this->connect(ui->doubleSpinBox,SIGNAL(valueChanged(double)),SLOT(onDoubleSpinBoxValueChanged(double)));
    ui->label_loud->setText(qApp->translate("PlaybackSoundSetupDialog","Loud"));
    ui->label_quiet->setText(qApp->translate("PlaybackSoundSetupDialog","Quiet"));
}

FaderVertical::~FaderVertical()
{
    delete ui;
}

float FaderVertical::getValue() const
{
    return static_cast< float >(ui->doubleSpinBox->value());
}

//! Retrieves slider change, converts to dB (float), sets spinbox
/*!
 * \brief FaderVertical::onSliderValueChanged Retrieves slider change, converts to dB (float), sets spinbox
 * \param val slider value (0-100)
 */
void FaderVertical::onSliderValueChanged(int val)
{
    double d_val = static_cast< double >(MMtoDB(static_cast< float >(val/100.)));
    d_val -= 10.0;
    d_val = d_val < -200.0 ? -200.0 : d_val;
    ui->doubleSpinBox->setValue(d_val);
}

//! Retrieves value changed from spinbox, sets slider if it's not the initial sender, emits ValueSet
/*!
 * \brief FaderVertical::onDoubleSpinBoxValueChanged Qt slot to retrieve value changed from spinbox
 * \param val -200 to 0
 */
void FaderVertical::onDoubleSpinBoxValueChanged(double val)
{
    //sender is doubleSpinBox, ergo update slider
    if (!(ui->horizontalSlider->isSliderDown()))   // loop breaker
        ui->horizontalSlider->setValue(static_cast< int >(100*DBtoMM(static_cast< float >(val+10.0))));

    emit valueChanged(static_cast< float >(val));
}
