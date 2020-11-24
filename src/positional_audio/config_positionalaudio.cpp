#include "config_positionalaudio.h"

#include <QtWidgets/QVBoxLayout>

#include "groupbox_positionalaudio.h"
#include "groupbox_positionalaudio_status.h"

ConfigPositionalAudio::ConfigPositionalAudio(QWidget *parent) :
    QDialog(parent)
{
    setAttribute( Qt::WA_DeleteOnClose );
    setWindowTitle(tr("Positional Audio"));
    //this->setFixedWidth(this->width());

    auto *layout = new QVBoxLayout(this);
    auto *groupbox_positionalaudio = new GroupBoxPositionalAudio(this);
    connect(groupbox_positionalaudio, &GroupBoxPositionalAudio::toggled, this, &ConfigPositionalAudio::EnabledSet);
    connect(groupbox_positionalaudio, &GroupBoxPositionalAudio::cameraSet, this, &ConfigPositionalAudio::CameraSet);

    connect(groupbox_positionalaudio, &GroupBoxPositionalAudio::attenuationSet, this, &ConfigPositionalAudio::AttenuationSet);
    connect(groupbox_positionalaudio, &GroupBoxPositionalAudio::distanceMinChanged, this, &ConfigPositionalAudio::DistanceMinChanged);
    connect(groupbox_positionalaudio, &GroupBoxPositionalAudio::distanceMaxChanged, this, &ConfigPositionalAudio::DistanceMaxChanged);
    connect(groupbox_positionalaudio, &GroupBoxPositionalAudio::rollOffChanged, this, &ConfigPositionalAudio::RollOffChanged);
    connect(groupbox_positionalaudio, &GroupBoxPositionalAudio::rollOffMaxChanged, this, &ConfigPositionalAudio::RollOffMaxChanged);

    connect(groupbox_positionalaudio, &GroupBoxPositionalAudio::ServerEnabledSet, this, &ConfigPositionalAudio::ServerEnabledSet);
    connect(groupbox_positionalaudio, &GroupBoxPositionalAudio::ServerSendIntervalChange, this, &ConfigPositionalAudio::ServerSendIntervalChange);
    connect(groupbox_positionalaudio, &GroupBoxPositionalAudio::ServerSendIntervalSilentIncChange, this, &ConfigPositionalAudio::ServerSendIntervalSilentIncChange);
    connect(groupbox_positionalaudio, &GroupBoxPositionalAudio::ServerBlockSet, this, &ConfigPositionalAudio::ServerBlockSet);
    connect(groupbox_positionalaudio, &GroupBoxPositionalAudio::ServerAddButtonClicked, this, &ConfigPositionalAudio::ServerAddButtonClicked);
    connect(groupbox_positionalaudio, &GroupBoxPositionalAudio::ServerRemoveButtonClicked, this, &ConfigPositionalAudio::ServerRemoveButtonClicked);

    // to UI
    connect(this, &ConfigPositionalAudio::UpdateUIEnabledSet, groupbox_positionalaudio, &GroupBoxPositionalAudio::setChecked);
    connect(this, &ConfigPositionalAudio::UpdateUIUseCameraSet, groupbox_positionalaudio, &GroupBoxPositionalAudio::UpdateUIUseCameraSet);

    connect(this, &ConfigPositionalAudio::UpdateUIUseAttenuationSet, groupbox_positionalaudio, &GroupBoxPositionalAudio::UpdateUIUseAttenuationSet);
    connect(this, &ConfigPositionalAudio::UpdateUIDistanceMin, groupbox_positionalaudio, &GroupBoxPositionalAudio::UpdateUIDistanceMin);
    connect(this, &ConfigPositionalAudio::UpdateUIDistanceMax, groupbox_positionalaudio, &GroupBoxPositionalAudio::UpdateUIDistanceMax);
    connect(this, &ConfigPositionalAudio::UpdateUIRollOff, groupbox_positionalaudio, &GroupBoxPositionalAudio::UpdateUIRollOff);
    connect(this, &ConfigPositionalAudio::UpdateUIRollOffMax, groupbox_positionalaudio, &GroupBoxPositionalAudio::UpdateUIRollOffMax);

    connect(this, &ConfigPositionalAudio::UpdateUIServerAdd, groupbox_positionalaudio, &GroupBoxPositionalAudio::UpdateUIServerAdd);
    connect(this, &ConfigPositionalAudio::UpdateUIServerRemove, groupbox_positionalaudio,&GroupBoxPositionalAudio::UpdateUIServerRemove);
    connect(this, &ConfigPositionalAudio::UpdateUIServerEnabled, groupbox_positionalaudio, &GroupBoxPositionalAudio::UpdateUIServerEnabled);
    connect(this, &ConfigPositionalAudio::UpdateUIServerBlocked, groupbox_positionalaudio, &GroupBoxPositionalAudio::UpdateUIServerBlocked);
    connect(this, &ConfigPositionalAudio::UpdateUIServerSendInterval, groupbox_positionalaudio, &GroupBoxPositionalAudio::UpdateUIServerSendInterval);
    connect(this, &ConfigPositionalAudio::UpdateUIServerSendIntervalSilentInc, groupbox_positionalaudio, &GroupBoxPositionalAudio::UpdateUIServerSendIntervalSilentInc);

    connect(this, &ConfigPositionalAudio::UpdateUIServerSelect, groupbox_positionalaudio, &GroupBoxPositionalAudio::UpdateUIServerSelect);
    layout->addWidget(groupbox_positionalaudio);

    auto *groupbox_positionalaudio_status = new GroupBoxPositionalAudioStatus(this);
    connect(this, &ConfigPositionalAudio::UpdateUIStatusSelfName, groupbox_positionalaudio_status, &GroupBoxPositionalAudioStatus::UpdateUIStatusSelfName);
    connect(this, &ConfigPositionalAudio::UpdateUIStatusSelfGame, groupbox_positionalaudio_status, &GroupBoxPositionalAudioStatus::UpdateUIStatusSelfGame);
    layout->addWidget(groupbox_positionalaudio_status);
    
    setLayout(layout);
}
