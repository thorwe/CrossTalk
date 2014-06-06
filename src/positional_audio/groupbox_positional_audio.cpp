#include "groupbox_positional_audio.h"
#include "ui_groupbox_positional_audio.h"

GroupBoxPositionalAudio::GroupBoxPositionalAudio(QWidget *parent) :
    QGroupBox(parent),
    ui(new Ui::GroupBoxPositionalAudio)
{
    ui->setupUi(this);

    //ui->groupBoxVolume->setEnabled(false);
    //ui->groupBoxMisc->setEnabled(false);
    //ui->groupBoxVolume->setVisible(false);
    //ui->groupBoxMisc->setVisible(false);

    connect(ui->pushButton_Camera,SIGNAL(toggled(bool)),this,SIGNAL(cameraSet(bool)));
    connect(ui->groupBoxVolume,SIGNAL(toggled(bool)),this,SIGNAL(attenuationSet(bool)));
    connect(ui->spinBox_MinDistance,SIGNAL(valueChanged(int)),this,SIGNAL(distanceMinChanged(int)));
    connect(ui->spinBox_MaxDistance,SIGNAL(valueChanged(int)),this,SIGNAL(distanceMaxChanged(int)));
    connect(ui->doubleSpinBox_RollOff,SIGNAL(valueChanged(double)),this,SIGNAL(rollOffChanged(float))); //let's see if that works
    connect(ui->doubleSpinBox_RollOffMax,SIGNAL(valueChanged(double)),this,SIGNAL(rollOffMaxChanged(float)));

    connect(ui->groupBoxServerSettings,SIGNAL(enabledSet(QString,bool)),this,SIGNAL(ServerEnabledSet(QString,bool)));
    connect(ui->groupBoxServerSettings,SIGNAL(sendIntervalChange(QString,float)),this,SIGNAL(ServerSendIntervalChange(QString,float)));
    connect(ui->groupBoxServerSettings,SIGNAL(sendIntervalSilentIncChange(QString,float)),this,SIGNAL(ServerSendIntervalSilentIncChange(QString,float)));
    connect(ui->groupBoxServerSettings,SIGNAL(blockedSet(QString,bool)),this,SIGNAL(ServerBlockSet(QString,bool)));
    connect(ui->groupBoxServerSettings,SIGNAL(addButtonClicked()),this,SIGNAL(ServerAddButtonClicked()));
    connect(ui->groupBoxServerSettings,SIGNAL(removeButtonClicked(QString)),this,SIGNAL(ServerRemoveButtonClicked(QString)));

    // to UI
    connect(this,SIGNAL(UpdateUIServerAdd(QString,QString)),ui->groupBoxServerSettings, SLOT(UpdateUIServerAdd(QString,QString)));
    connect(this,SIGNAL(UpdateUIServerRemove(QString,QString)),ui->groupBoxServerSettings, SLOT(UpdateUIServerRemove(QString,QString)));
    connect(this,SIGNAL(UpdateUIServerEnabled(QString,bool)), ui->groupBoxServerSettings, SLOT(UpdateUIServerEnabled(QString,bool)));
    connect(this,SIGNAL(UpdateUIServerBlocked(QString,bool)), ui->groupBoxServerSettings, SLOT(UpdateUIServerBlocked(QString,bool)));
    connect(this,SIGNAL(UpdateUIServerSendInterval(QString,float)),ui->groupBoxServerSettings, SLOT(UpdateUIServerSendInterval(QString,float)));
    connect(this,SIGNAL(UpdateUIServerSendIntervalSilentInc(QString,float)), ui->groupBoxServerSettings, SLOT(UpdateUIServerSendIntervalSilentInc(QString,float)));

    connect(this,SIGNAL(UpdateUIServerSelect(QString)),ui->groupBoxServerSettings, SLOT(UpdateUIServerSelect(QString)));
}

GroupBoxPositionalAudio::~GroupBoxPositionalAudio()
{
    delete ui;
}

void GroupBoxPositionalAudio::UpdateUIUseCameraSet(bool val)
{
    this->blockSignals(true);
    ui->pushButton_Camera->setChecked(val);
    ui->pushButton_Avatar->setChecked(!val);
    this->blockSignals(false);
}

void GroupBoxPositionalAudio::UpdateUIUseAttenuationSet(bool val)
{
    this->blockSignals(true);
    ui->groupBoxVolume->setChecked(val);
    this->blockSignals(false);
}

void GroupBoxPositionalAudio::UpdateUIDistanceMin(int val)
{
    this->blockSignals(true);
    ui->spinBox_MinDistance->setValue(val);
    this->blockSignals(false);
}

void GroupBoxPositionalAudio::UpdateUIDistanceMax(int val)
{
    this->blockSignals(true);
    ui->spinBox_MaxDistance->setValue(val);
    this->blockSignals(false);
}

void GroupBoxPositionalAudio::UpdateUIRollOff(float val)
{
    this->blockSignals(true);
    ui->doubleSpinBox_RollOff->setValue(val);
    this->blockSignals(false);
}

void GroupBoxPositionalAudio::UpdateUIRollOffMax(float val)
{
    this->blockSignals(true);
    ui->doubleSpinBox_RollOffMax->setValue(val);
    this->blockSignals(false);
}
