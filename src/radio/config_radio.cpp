#include "config_radio.h"
#include "ui_config_radio.h"

#include "ts_helpers_qt.h"
#include <QPushButton>

ConfigRadio::ConfigRadio(QWidget *parent, QString title) :
    QDialog(parent), //ui(new Ui::ConfigRadio)
    m_title(title)
{
    if (title.isEmpty())  // global settings
    {
        ui = new Ui::ConfigRadio();
        ui->setupUi(this);
        this->setFixedSize(this->width(),this->height());
    }
    this->setAttribute( Qt::WA_DeleteOnClose );
}

ConfigRadio::~ConfigRadio()
{
    if (ui != nullptr)
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
    channelStrip->setTitle((m_title.isEmpty()) ? name : m_title);

    connect(channelStrip, &ConfigRadioGroupBox::EnabledSet, this, &ConfigRadio::EnabledSet);
    connect(channelStrip, &ConfigRadioGroupBox::InLoFreqSet, this, &ConfigRadio::InLoFreqSet);
    connect(channelStrip, &ConfigRadioGroupBox::InHiFreqSet, this, &ConfigRadio::InHiFreqSet);
    connect(channelStrip, &ConfigRadioGroupBox::DestructionSet, this, &ConfigRadio::DestructionSet);
    connect(channelStrip, &ConfigRadioGroupBox::RingModFrequencySet, this, &ConfigRadio::RingModFrequencySet);
    connect(channelStrip, &ConfigRadioGroupBox::RingModMixSet, this, &ConfigRadio::RingModMixSet);
    connect(channelStrip, &ConfigRadioGroupBox::OutLoFreqSet, this, &ConfigRadio::OutLoFreqSet);
    connect(channelStrip, &ConfigRadioGroupBox::OutHiFreqSet, this, &ConfigRadio::OutHiFreqSet);

    if (ui != nullptr)
        ui->horizontalLayout->addWidget(channelStrip);
    else
    {
        m_name = name;
        QVBoxLayout *layout = new QVBoxLayout;
        layout->addWidget(channelStrip);
        QPushButton* remove_button = new QPushButton(this);
        QIcon remove_icon(":/icons/delete.png");
        remove_button->setIcon(remove_icon);
        remove_button->setAccessibleName("Delete");
        remove_button->setText("Delete");
        connect(remove_button, &QPushButton::clicked, [=] (bool checked)
        {
            Q_UNUSED(checked);
            // since the x button occupies Rejected, our bool means isDelete
            this->done(QDialog::DialogCode::Accepted);
        });
        layout->addWidget(remove_button, 0, Qt::AlignCenter);
        this->setLayout(layout);
        this->adjustSize();
        this->setFixedSize(this->width(),this->height());
        this->connect(this, &ConfigRadio::finished, [=](int r)
        {
            emit channel_closed(r, m_name); // emit in lambda, nice :)
        });
    }
    m_ChannelStripMap.insert(name,channelStrip);

    return channelStrip;
}
