#include "config_positionalaudio.h"
#include "ui_config_positionalaudio.h"

ConfigPositionalAudio::ConfigPositionalAudio(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConfigPositionalAudio)
{
    ui->setupUi(this);
    this->setAttribute( Qt::WA_DeleteOnClose );
    this->setFixedWidth(this->width());

    connect(ui->groupBoxPositionalAudio, &GroupBoxPositionalAudio::toggled, this, &ConfigPositionalAudio::EnabledSet);
    connect(ui->groupBoxPositionalAudio, &GroupBoxPositionalAudio::cameraSet, this, &ConfigPositionalAudio::CameraSet);

    connect(ui->groupBoxPositionalAudio, &GroupBoxPositionalAudio::attenuationSet, this, &ConfigPositionalAudio::AttenuationSet);
    connect(ui->groupBoxPositionalAudio, &GroupBoxPositionalAudio::distanceMinChanged, this, &ConfigPositionalAudio::DistanceMinChanged);
    connect(ui->groupBoxPositionalAudio, &GroupBoxPositionalAudio::distanceMaxChanged, this, &ConfigPositionalAudio::DistanceMaxChanged);
    connect(ui->groupBoxPositionalAudio, &GroupBoxPositionalAudio::rollOffChanged, this, &ConfigPositionalAudio::RollOffChanged);
    connect(ui->groupBoxPositionalAudio, &GroupBoxPositionalAudio::rollOffMaxChanged, this, &ConfigPositionalAudio::RollOffMaxChanged);

    connect(ui->groupBoxPositionalAudio, &GroupBoxPositionalAudio::ServerEnabledSet, this, &ConfigPositionalAudio::ServerEnabledSet);
    connect(ui->groupBoxPositionalAudio, &GroupBoxPositionalAudio::ServerSendIntervalChange, this, &ConfigPositionalAudio::ServerSendIntervalChange);
    connect(ui->groupBoxPositionalAudio, &GroupBoxPositionalAudio::ServerSendIntervalSilentIncChange, this, &ConfigPositionalAudio::ServerSendIntervalSilentIncChange);
    connect(ui->groupBoxPositionalAudio, &GroupBoxPositionalAudio::ServerBlockSet, this, &ConfigPositionalAudio::ServerBlockSet);
    connect(ui->groupBoxPositionalAudio, &GroupBoxPositionalAudio::ServerAddButtonClicked, this, &ConfigPositionalAudio::ServerAddButtonClicked);
    connect(ui->groupBoxPositionalAudio, &GroupBoxPositionalAudio::ServerRemoveButtonClicked, this, &ConfigPositionalAudio::ServerRemoveButtonClicked);

    // to UI
    connect(this, &ConfigPositionalAudio::UpdateUIEnabledSet, ui->groupBoxPositionalAudio, &GroupBoxPositionalAudio::setChecked);
    connect(this, &ConfigPositionalAudio::UpdateUIUseCameraSet, ui->groupBoxPositionalAudio, &GroupBoxPositionalAudio::UpdateUIUseCameraSet);

    connect(this, &ConfigPositionalAudio::UpdateUIUseAttenuationSet, ui->groupBoxPositionalAudio, &GroupBoxPositionalAudio::UpdateUIUseAttenuationSet);
    connect(this, &ConfigPositionalAudio::UpdateUIDistanceMin, ui->groupBoxPositionalAudio, &GroupBoxPositionalAudio::UpdateUIDistanceMin);
    connect(this, &ConfigPositionalAudio::UpdateUIDistanceMax, ui->groupBoxPositionalAudio, &GroupBoxPositionalAudio::UpdateUIDistanceMax);
    connect(this, &ConfigPositionalAudio::UpdateUIRollOff, ui->groupBoxPositionalAudio, &GroupBoxPositionalAudio::UpdateUIRollOff);
    connect(this, &ConfigPositionalAudio::UpdateUIRollOffMax, ui->groupBoxPositionalAudio, &GroupBoxPositionalAudio::UpdateUIRollOffMax);

    connect(this, &ConfigPositionalAudio::UpdateUIServerAdd, ui->groupBoxPositionalAudio, &GroupBoxPositionalAudio::UpdateUIServerAdd);
    connect(this, &ConfigPositionalAudio::UpdateUIServerRemove, ui->groupBoxPositionalAudio,&GroupBoxPositionalAudio::UpdateUIServerRemove);
    connect(this, &ConfigPositionalAudio::UpdateUIServerEnabled, ui->groupBoxPositionalAudio, &GroupBoxPositionalAudio::UpdateUIServerEnabled);
    connect(this, &ConfigPositionalAudio::UpdateUIServerBlocked, ui->groupBoxPositionalAudio, &GroupBoxPositionalAudio::UpdateUIServerBlocked);
    connect(this, &ConfigPositionalAudio::UpdateUIServerSendInterval, ui->groupBoxPositionalAudio, &GroupBoxPositionalAudio::UpdateUIServerSendInterval);
    connect(this, &ConfigPositionalAudio::UpdateUIServerSendIntervalSilentInc, ui->groupBoxPositionalAudio, &GroupBoxPositionalAudio::UpdateUIServerSendIntervalSilentInc);

    connect(this, &ConfigPositionalAudio::UpdateUIServerSelect, ui->groupBoxPositionalAudio, &GroupBoxPositionalAudio::UpdateUIServerSelect);

    connect(this, &ConfigPositionalAudio::UpdateUIStatusSelfName, ui->groupBoxPositionalAudioStatus, &GroupBoxPositionalAudioStatus::UpdateUIStatusSelfName);
    connect(this, &ConfigPositionalAudio::UpdateUIStatusSelfGame, ui->groupBoxPositionalAudioStatus, &GroupBoxPositionalAudioStatus::UpdateUIStatusSelfGame);
}

ConfigPositionalAudio::~ConfigPositionalAudio()
{
    delete ui;
}
