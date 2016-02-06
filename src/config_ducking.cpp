#include "config_ducking.h"
#include "ui_config_ducking.h"

#include <QVBoxLayout>
#include <QGridLayout>
#include <QLabel>

ConfigDucking::ConfigDucking(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConfigDucking)
{
    ui->setupUi(this);
    this->setAttribute( Qt::WA_DeleteOnClose );
    this->setFixedSize(this->width(),this->height());

    // Channel Ducking
    c = new QGroupBox("Channel",this);
    c->setCheckable(true);
    this->connect(c, &QGroupBox::toggled, this, &ConfigDucking::channelDuckerEnabledSet);

    QVBoxLayout* cLayout = new QVBoxLayout();

    QGridLayout* cTargetLayout = new QGridLayout();

    QString text = QString("<b>%1</b>").arg(tr("Target:"));
    QLabel* cTargetLabel = new QLabel(text);
    cTargetLayout->addWidget(cTargetLabel,0,0,Qt::AlignCenter);

    cRadioTargetCurrent = new QRadioButton(qApp->translate("HotkeyDialog","Current Server"));
    connect(cRadioTargetCurrent, &QRadioButton::toggled, this, &ConfigDucking::onCRadioTargetCurrentToggled);
    cTargetLayout->addWidget(cRadioTargetCurrent,0,1,Qt::AlignLeft);

    QString otherServers = qApp->translate("NotificationsSetup","Other");
    otherServers.append(" ");
    otherServers.append(qApp->translate("ImprovedTabBar", "Server Tabs"));
    cRadioTargetOther = new QRadioButton(otherServers);
    connect(cRadioTargetOther, &QRadioButton::toggled, this, &ConfigDucking::onCRadioTargetOtherToggled);
    cTargetLayout->addWidget(cRadioTargetOther,1,1,Qt::AlignLeft);

    //cTargetLayout->setColumnStretch(0,1);
    //cTargetLayout->setColumnStretch(1,2);
    cTargetLayout->setColumnMinimumWidth(1,130);
    cLayout->addLayout(cTargetLayout);

    cFader = new FaderVertical();
    connect(cFader, &FaderVertical::valueChanged, this, &ConfigDucking::channelDuckerValueChanged);
    cLayout->addWidget(cFader,0,Qt::AlignCenter);

    cPS = new QCheckBox("Duck Priority Speakers");
    connect(cPS, &QCheckBox::toggled, this, &ConfigDucking::channelDuckerDuckPSEnabledSet);
    cLayout->addWidget(cPS,0,Qt::AlignCenter);

    c->setLayout(cLayout);

    // Global Ducking
    g = new QGroupBox("Global (music bots)",this);
    g->setCheckable(true);
    connect(g, &QGroupBox::toggled, this, &ConfigDucking::globalDuckerEnabledSet);

    QVBoxLayout* gLayout = new QVBoxLayout();
    gFader = new FaderVertical();
    connect(gFader, &FaderVertical::valueChanged, this, &ConfigDucking::globalDuckerValueChanged);
    gLayout->addWidget(gFader,0,Qt::AlignCenter);

    gLayout->addStretch(1);
    g->setLayout(gLayout);

    ui->horizontalLayout->addWidget(g);
    ui->horizontalLayout->addWidget(c);

    gLayout->insertSpacing(0,46);
}

ConfigDucking::~ConfigDucking()
{
    delete ui;
}

void ConfigDucking::UpdateGlobalDuckerEnabled(bool val)
{
    g->blockSignals(true);
    g->setChecked(val);
    g->blockSignals(false);
}

void ConfigDucking::UpdateGlobalDuckerValue(float val)
{
    gFader->blockSignals(true);
    gFader->setValue(val);
    gFader->blockSignals(false);
}

void ConfigDucking::UpdateChannelDuckerEnabled(bool val)
{
    c->blockSignals(true);
    c->setChecked(val);
    c->blockSignals(false);
}

void ConfigDucking::UpdateChannelDuckerValue(float val)
{
    cFader->blockSignals(true);
    cFader->setValue(val);
    cFader->blockSignals(false);
}

void ConfigDucking::UpdateChannelDuckerReverse(bool val)
{
    cRadioTargetOther->blockSignals(true);
    cRadioTargetCurrent->blockSignals(true);
    cRadioTargetOther->setChecked(val);
    cRadioTargetCurrent->setChecked(!val);
    cRadioTargetOther->blockSignals(false);
    cRadioTargetCurrent->blockSignals(false);
}

void ConfigDucking::UpdateChannelDuckerDuckPSEnabled(bool val)
{
    cPS->blockSignals(true);
    cPS->setChecked(val);
    cPS->blockSignals(false);
}

void ConfigDucking::onCRadioTargetCurrentToggled(bool val)
{
    cRadioTargetOther->blockSignals(true);
    cRadioTargetOther->setChecked(!val);
    cRadioTargetOther->blockSignals(false);
    emit channelDuckerReverseSet(!val);
}

void ConfigDucking::onCRadioTargetOtherToggled(bool val)
{
    cRadioTargetCurrent->blockSignals(true);
    cRadioTargetCurrent->setChecked(!val);
    cRadioTargetCurrent->blockSignals(false);
    emit channelDuckerReverseSet(val);
}
