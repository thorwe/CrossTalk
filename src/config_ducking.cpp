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
    this->connect(c,SIGNAL(toggled(bool)),SIGNAL(channelDuckerEnabledSet(bool)));

    QVBoxLayout* cLayout = new QVBoxLayout();

    QGridLayout* cTargetLayout = new QGridLayout();

    QString text = QString("<b>%1</b>").arg(tr("Target:"));
    QLabel* cTargetLabel = new QLabel(text);
    cTargetLayout->addWidget(cTargetLabel,0,0,Qt::AlignCenter);

    cRadioTargetCurrent = new QRadioButton(qApp->translate("HotkeyDialog","Current Server"));
    this->connect(cRadioTargetCurrent,SIGNAL(toggled(bool)),this,SLOT(onCRadioTargetCurrentToggled(bool)));
    cTargetLayout->addWidget(cRadioTargetCurrent,0,1,Qt::AlignLeft);

    QString otherServers = qApp->translate("NotificationsSetup","Other");
    otherServers.append(" ");
    otherServers.append(qApp->translate("ImprovedTabBar", "Server Tabs"));
    cRadioTargetOther = new QRadioButton(otherServers);
    this->connect(cRadioTargetOther,SIGNAL(toggled(bool)),this,SLOT(onCRadioTargetOtherToggled(bool)));
    cTargetLayout->addWidget(cRadioTargetOther,1,1,Qt::AlignLeft);

    //cTargetLayout->setColumnStretch(0,1);
    //cTargetLayout->setColumnStretch(1,2);
    cTargetLayout->setColumnMinimumWidth(1,130);
    cLayout->addLayout(cTargetLayout);

    cFader = new FaderVertical();
    this->connect(cFader,SIGNAL(valueChanged(float)),SIGNAL(channelDuckerValueChanged(float)));
    cLayout->addWidget(cFader,0,Qt::AlignCenter);

    cPS = new QCheckBox("Duck Priority Speakers");
    this->connect(cPS,SIGNAL(toggled(bool)),SIGNAL(channelDuckerDuckPSEnabledSet(bool)));
    cLayout->addWidget(cPS,0,Qt::AlignCenter);

    c->setLayout(cLayout);

    // Global Ducking
    g = new QGroupBox("Global (music bots)",this);
    g->setCheckable(true);
    this->connect(g,SIGNAL(toggled(bool)),SIGNAL(globalDuckerEnabledSet(bool)));

    QVBoxLayout* gLayout = new QVBoxLayout();
    gFader = new FaderVertical();
    this->connect(gFader,SIGNAL(valueChanged(float)),SIGNAL(globalDuckerValueChanged(float)));
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
