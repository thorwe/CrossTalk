#include "config_radio_channelstrip.h"
#include "ui_config_radio_channelstrip.h"

ConfigRadioChannelStrip::ConfigRadioChannelStrip(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ConfigRadioChannelStrip)
{
    ui->setupUi(this);
}

ConfigRadioChannelStrip::~ConfigRadioChannelStrip()
{
    delete ui;
}
