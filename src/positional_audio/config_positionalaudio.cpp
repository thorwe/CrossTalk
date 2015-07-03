#include "config_positionalaudio.h"
#include "ui_config_positionalaudio.h"

ConfigPositionalAudio::ConfigPositionalAudio(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConfigPositionalAudio)
{
    ui->setupUi(this);
    this->setAttribute( Qt::WA_DeleteOnClose );
    this->setFixedWidth(this->width());

    connect(ui->groupBoxPositionalAudio,SIGNAL(toggled(bool)),this,SIGNAL(EnabledSet(bool)));
    connect(ui->groupBoxPositionalAudio,SIGNAL(cameraSet(bool)),this,SIGNAL(CameraSet(bool)));

    connect(ui->groupBoxPositionalAudio,SIGNAL(attenuationSet(bool)),this,SIGNAL(AttenuationSet(bool)));
    connect(ui->groupBoxPositionalAudio,SIGNAL(distanceMinChanged(int)),this,SIGNAL(DistanceMinChanged(int)));
    connect(ui->groupBoxPositionalAudio,SIGNAL(distanceMaxChanged(int)),this,SIGNAL(DistanceMaxChanged(int)));
    connect(ui->groupBoxPositionalAudio,SIGNAL(rollOffChanged(float)),this,SIGNAL(RollOffChanged(float)));
    connect(ui->groupBoxPositionalAudio,SIGNAL(rollOffMaxChanged(float)),this,SIGNAL(RollOffMaxChanged(float)));

    connect(ui->groupBoxPositionalAudio,SIGNAL(ServerEnabledSet(QString,bool)),this,SIGNAL(ServerEnabledSet(QString,bool)));
    connect(ui->groupBoxPositionalAudio,SIGNAL(ServerSendIntervalChange(QString,float)),this,SIGNAL(ServerSendIntervalChange(QString,float)));
    connect(ui->groupBoxPositionalAudio,SIGNAL(ServerSendIntervalSilentIncChange(QString,float)),this,SIGNAL(ServerSendIntervalSilentIncChange(QString,float)));
    connect(ui->groupBoxPositionalAudio,SIGNAL(ServerBlockSet(QString,bool)),this,SIGNAL(ServerBlockSet(QString,bool)));
    connect(ui->groupBoxPositionalAudio,SIGNAL(ServerAddButtonClicked()),this,SIGNAL(ServerAddButtonClicked()));
    connect(ui->groupBoxPositionalAudio,SIGNAL(ServerRemoveButtonClicked(QString)),this,SIGNAL(ServerRemoveButtonClicked(QString)));

    // to UI
    this->connect(this,SIGNAL(UpdateUIEnabledSet(bool)),ui->groupBoxPositionalAudio,SLOT(setChecked(bool)));
    this->connect(this,SIGNAL(UpdateUIUseCameraSet(bool)),ui->groupBoxPositionalAudio,SLOT(UpdateUIUseCameraSet(bool)));

    this->connect(this,SIGNAL(UpdateUIUseAttenuationSet(bool)),ui->groupBoxPositionalAudio,SLOT(UpdateUIUseAttenuationSet(bool)));
    this->connect(this,SIGNAL(UpdateUIDistanceMin(int)),ui->groupBoxPositionalAudio,SLOT(UpdateUIDistanceMin(int)));
    this->connect(this,SIGNAL(UpdateUIDistanceMax(int)),ui->groupBoxPositionalAudio,SLOT(UpdateUIDistanceMax(int)));
    this->connect(this,SIGNAL(UpdateUIRollOff(float)),ui->groupBoxPositionalAudio,SLOT(UpdateUIRollOff(float)));
    this->connect(this,SIGNAL(UpdateUIRollOffMax(float)),ui->groupBoxPositionalAudio,SLOT(UpdateUIRollOffMax(float)));


    this->connect(this,SIGNAL(UpdateUIServerAdd(QString,QString)),ui->groupBoxPositionalAudio,SIGNAL(UpdateUIServerAdd(QString,QString)));
    this->connect(this,SIGNAL(UpdateUIServerRemove(QString,QString)),ui->groupBoxPositionalAudio,SIGNAL(UpdateUIServerRemove(QString,QString)));
    this->connect(this,SIGNAL(UpdateUIServerEnabled(QString,bool)),ui->groupBoxPositionalAudio,SIGNAL(UpdateUIServerEnabled(QString,bool)));
    this->connect(this,SIGNAL(UpdateUIServerBlocked(QString,bool)),ui->groupBoxPositionalAudio,SIGNAL(UpdateUIServerBlocked(QString,bool)));
    this->connect(this,SIGNAL(UpdateUIServerSendInterval(QString,float)),ui->groupBoxPositionalAudio,SIGNAL(UpdateUIServerSendInterval(QString,float)));
    this->connect(this,SIGNAL(UpdateUIServerSendIntervalSilentInc(QString,float)),ui->groupBoxPositionalAudio,SIGNAL(UpdateUIServerSendIntervalSilentInc(QString,float)));

    this->connect(this,SIGNAL(UpdateUIServerSelect(QString)),ui->groupBoxPositionalAudio,SIGNAL(UpdateUIServerSelect(QString)));

    this->connect(this,SIGNAL(UpdateUIStatusSelfName(QString)),ui->groupBoxPositionalAudioStatus,SLOT(UpdateUIStatusSelfName(QString)));
    this->connect(this,SIGNAL(UpdateUIStatusSelfGame(QString)),ui->groupBoxPositionalAudioStatus,SLOT(UpdateUIStatusSelfGame(QString)));
}

ConfigPositionalAudio::~ConfigPositionalAudio()
{
    delete ui;
}
