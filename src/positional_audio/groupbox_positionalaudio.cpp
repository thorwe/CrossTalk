#include "groupbox_positionalaudio.h"

#include "groupbox_positionalaudio_servers.h"

#include <QtWidgets/QFormLayout>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpinBox>

GroupBoxPositionalAudio::GroupBoxPositionalAudio(QWidget *parent)
    : QGroupBox(parent)
{
    setTitle(tr("Positional Audio"));

    auto layout = new QGridLayout(this);

    auto groupbox_listener = new QGroupBox(this);
    {
        groupbox_listener->setTitle(tr("Listener"));
        auto listener_layout = new QHBoxLayout;

        auto get_button = []() -> QPushButton * {
            auto button = new QPushButton;
            button->setFixedSize(66, 66);
            button->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
            button->setIconSize(QSize(64, 64));
            button->setCheckable(true);
            button->setChecked(false);
            button->setAutoExclusive(true);
            button->setAutoDefault(false);
            return button;
        };
        m_avatar = get_button();
        m_avatar->setToolTip(tr("Avatar"));
        m_avatar->setIcon(QIcon(":/positionalaudio/avatar"));
        listener_layout->addWidget(m_avatar);

        m_camera = get_button();
        m_camera->setToolTip(tr("Camera"));
        m_camera->setIcon(QIcon(":/positionalaudio/camera"));
        connect(m_camera, &QPushButton::toggled, this, &GroupBoxPositionalAudio::cameraSet);
        listener_layout->addWidget(m_camera);

        groupbox_listener->setLayout(listener_layout);
    }
    layout->addWidget(groupbox_listener, 0, 0);

    m_volume = new QGroupBox;
    {
        m_volume->setTitle(tr("Volume Attenuation"));
        m_volume->setCheckable(true);
        m_volume->setChecked(false);
        connect(m_volume, &QGroupBox::toggled, this, &GroupBoxPositionalAudio::attenuationSet);

        auto volume_layout = new QFormLayout;

        m_distance_min = new QSpinBox;
        m_distance_min->setSuffix(QStringLiteral(" m"));
        connect(m_distance_min, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this,
                &GroupBoxPositionalAudio::distanceMinChanged);
        volume_layout->addRow(tr("Minimum distance"), m_distance_min);

        m_distance_max = new QSpinBox;
        m_distance_max->setSuffix(QStringLiteral(" m"));
        m_distance_max->setMaximum(9999);
        connect(m_distance_max, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this,
                &GroupBoxPositionalAudio::distanceMaxChanged);
        volume_layout->addRow(tr("Maximum distance"), m_distance_max);

        m_rolloff = new QDoubleSpinBox;
        m_rolloff->setSuffix(QStringLiteral(" dB"));
        m_rolloff->setMinimum(-200);
        m_rolloff->setMaximum(0);
        connect(m_rolloff, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this,
                &GroupBoxPositionalAudio::on_DoubleSpinBoxRollOff_valueChanged);
        volume_layout->addRow(tr("RollOff"), m_rolloff);

        m_rolloff_max = new QDoubleSpinBox;
        m_rolloff_max->setSuffix(QStringLiteral(" dB"));
        m_rolloff_max->setMinimum(-200);
        m_rolloff_max->setMaximum(0);
        connect(m_rolloff_max, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
                this, &GroupBoxPositionalAudio::on_DoubleSpinBoxRollOffMax_valueChanged);
        volume_layout->addRow(tr("Maximum RollOff"), m_rolloff_max);

        m_volume->setLayout(volume_layout);
    }
    layout->addWidget(m_volume, 1, 0);

    // group box server settings
    {
        auto groupbox_serversettings = new GroupBoxPositionalAudioServers;
        connect(groupbox_serversettings, &GroupBoxPositionalAudioServers::enabledSet, this,
                &GroupBoxPositionalAudio::ServerEnabledSet);
        connect(groupbox_serversettings, &GroupBoxPositionalAudioServers::sendIntervalChange, this,
                &GroupBoxPositionalAudio::ServerSendIntervalChange);
        connect(groupbox_serversettings, &GroupBoxPositionalAudioServers::sendIntervalSilentIncChange, this,
                &GroupBoxPositionalAudio::ServerSendIntervalSilentIncChange);
        connect(groupbox_serversettings, &GroupBoxPositionalAudioServers::blockedSet, this,
                &GroupBoxPositionalAudio::ServerBlockSet);
        connect(groupbox_serversettings, &GroupBoxPositionalAudioServers::addButtonClicked, this,
                &GroupBoxPositionalAudio::ServerAddButtonClicked);
        connect(groupbox_serversettings, &GroupBoxPositionalAudioServers::removeButtonClicked, this,
                &GroupBoxPositionalAudio::ServerRemoveButtonClicked);

        // to UI
        connect(this, &GroupBoxPositionalAudio::UpdateUIServerAdd, groupbox_serversettings,
                &GroupBoxPositionalAudioServers::UpdateUIServerAdd);
        connect(this, &GroupBoxPositionalAudio::UpdateUIServerRemove, groupbox_serversettings,
                &GroupBoxPositionalAudioServers::UpdateUIServerRemove);
        connect(this, &GroupBoxPositionalAudio::UpdateUIServerEnabled, groupbox_serversettings,
                &GroupBoxPositionalAudioServers::UpdateUIServerEnabled);
        connect(this, &GroupBoxPositionalAudio::UpdateUIServerBlocked, groupbox_serversettings,
                &GroupBoxPositionalAudioServers::UpdateUIServerBlocked);
        connect(this, &GroupBoxPositionalAudio::UpdateUIServerSendInterval, groupbox_serversettings,
                &GroupBoxPositionalAudioServers::UpdateUIServerSendInterval);
        connect(this, &GroupBoxPositionalAudio::UpdateUIServerSendIntervalSilentInc, groupbox_serversettings,
                &GroupBoxPositionalAudioServers::UpdateUIServerSendIntervalSilentInc);

        connect(this, &GroupBoxPositionalAudio::UpdateUIServerSelect, groupbox_serversettings,
                &GroupBoxPositionalAudioServers::UpdateUIServerSelect);

        layout->addWidget(groupbox_serversettings, 0, 1, 2, 1);
    }
}

void GroupBoxPositionalAudio::UpdateUIUseCameraSet(bool val)
{
    blockSignals(true);
    m_camera->setChecked(val);
    m_avatar->setChecked(!val);
    blockSignals(false);
}

void GroupBoxPositionalAudio::UpdateUIUseAttenuationSet(bool val)
{
    blockSignals(true);
    m_volume->setChecked(val);
    blockSignals(false);
}

void GroupBoxPositionalAudio::UpdateUIDistanceMin(int val)
{
    blockSignals(true);
    m_distance_min->setValue(val);
    blockSignals(false);
}

void GroupBoxPositionalAudio::UpdateUIDistanceMax(int val)
{
    blockSignals(true);
    m_distance_max->setValue(val);
    blockSignals(false);
}

void GroupBoxPositionalAudio::UpdateUIRollOff(float val)
{
    blockSignals(true);
    m_rolloff->setValue(val);
    blockSignals(false);
}

void GroupBoxPositionalAudio::UpdateUIRollOffMax(float val)
{
    blockSignals(true);
    m_rolloff_max->setValue(val);
    blockSignals(false);
}

void GroupBoxPositionalAudio::on_DoubleSpinBoxRollOff_valueChanged(double val)
{
    emit rollOffChanged(static_cast<float>(val));
}

void GroupBoxPositionalAudio::on_DoubleSpinBoxRollOffMax_valueChanged(double val)
{
    emit rollOffMaxChanged(static_cast<float>(val));
}
