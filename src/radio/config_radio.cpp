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
    auto channel_strip = GetChannelStrip(name);
    channel_strip->blockSignals(true);
    channel_strip->setChecked(val);
    channel_strip->blockSignals(false);
}

void ConfigRadio::UpdateBandpassInLowFrequency(QString name, double val)
{
    auto channel_strip = GetChannelStrip(name);
    channel_strip->blockSignals(true);
    channel_strip->onInLoValueChanged(val);
    channel_strip->blockSignals(false);
}

void ConfigRadio::UpdateBandpassInHighFrequency(QString name, double val)
{
    auto channel_strip = GetChannelStrip(name);
    channel_strip->blockSignals(true);
    channel_strip->onInHiValueChanged(val);
    channel_strip->blockSignals(false);
}

void ConfigRadio::UpdateDestruction(QString name, double val)
{
    auto channel_strip = GetChannelStrip(name);
    channel_strip->blockSignals(true);
    channel_strip->onDestrValueChanged(val);
    channel_strip->blockSignals(false);
}

void ConfigRadio::UpdateRingModFrequency(QString name, double val)
{
    auto channel_strip = GetChannelStrip(name);
    channel_strip->blockSignals(true);
    channel_strip->onRingModFrequencyValueChanged(val);
    channel_strip->blockSignals(false);
}

void ConfigRadio::UpdateRingModMix(QString name, double val)
{
    auto channel_strip = GetChannelStrip(name);
    channel_strip->blockSignals(true);
    channel_strip->onRingModMixValueChanged(val);
    channel_strip->blockSignals(false);
}

void ConfigRadio::UpdateBandpassOutLowFrequency(QString name, double val)
{
    auto channel_strip = GetChannelStrip(name);
    channel_strip->blockSignals(true);
    channel_strip->onOutLoValueChanged(val);
    channel_strip->blockSignals(false);
}

void ConfigRadio::UpdateBandpassOutHighFrequency(QString name, double val)
{
    auto channel_strip = GetChannelStrip(name);
    channel_strip->blockSignals(true);
    channel_strip->onOutHiValueChanged(val);
    channel_strip->blockSignals(false);
}

ConfigRadioGroupBox* ConfigRadio::GetChannelStrip(QString name)
{
    if (m_channelstrips.contains(name))
        return m_channelstrips.value(name);

    auto channel_strip = new ConfigRadioGroupBox(this);
    channel_strip->setObjectName(name);
    channel_strip->setTitle((m_title.isEmpty()) ? name : m_title);

    connect(channel_strip, &ConfigRadioGroupBox::EnabledSet, this, &ConfigRadio::EnabledSet);
    connect(channel_strip, &ConfigRadioGroupBox::InLoFreqSet, this, &ConfigRadio::InLoFreqSet);
    connect(channel_strip, &ConfigRadioGroupBox::InHiFreqSet, this, &ConfigRadio::InHiFreqSet);
    connect(channel_strip, &ConfigRadioGroupBox::DestructionSet, this, &ConfigRadio::DestructionSet);
    connect(channel_strip, &ConfigRadioGroupBox::RingModFrequencySet, this, &ConfigRadio::RingModFrequencySet);
    connect(channel_strip, &ConfigRadioGroupBox::RingModMixSet, this, &ConfigRadio::RingModMixSet);
    connect(channel_strip, &ConfigRadioGroupBox::OutLoFreqSet, this, &ConfigRadio::OutLoFreqSet);
    connect(channel_strip, &ConfigRadioGroupBox::OutHiFreqSet, this, &ConfigRadio::OutHiFreqSet);

    if (ui)
        ui->horizontalLayout->addWidget(channel_strip);
    else
    {
        m_name = name;
        auto layout = new QVBoxLayout;
        layout->addWidget(channel_strip);
        auto remove_button = new QPushButton(this);
        QIcon remove_icon(QStringLiteral(":/icons/delete.png"));
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
            emit channel_closed(r, m_name);
        });
    }
    m_channelstrips.insert(name,channel_strip);

    return channel_strip;
}
