#include "groupbox_positionalaudio_status.h"

#include <QtWidgets/QGridLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>

#include "core/ts_settings_qt.h"

GroupBoxPositionalAudioStatus::GroupBoxPositionalAudioStatus(QWidget *parent)
    : QGroupBox(parent)
{
    setTitle(tr("Status"));

    auto layout = new QGridLayout(this);

    auto get_icon_label = []() -> QLabel * {
        auto icon = new QLabel;
        icon->setMinimumSize(24, 24);
        icon->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        icon->setAlignment(Qt::AlignCenter);
        return icon;
    };
    auto icon = get_icon_label();
    QPixmap threedeepixmap(":/icons/3d_sound.png");  //"gfx/default/24x24_3d_sound.png" also 16x16
    icon->setPixmap(threedeepixmap);
    layout->addWidget(icon, 0, 0, Qt::AlignCenter);

    m_status_settings_threedee = new QLabel(tr("Unknown status."), this);
    m_status_settings_threedee->setWordWrap(true);
    layout->addWidget(m_status_settings_threedee, 0, 1, Qt::AlignLeft);

    auto refresh = new QPushButton(this);
    refresh->setMinimumSize(16, 16);
    refresh->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    // 20x20_3d_sound_user
    QIcon threedeerefresh(":/icons/check_update.png");  //"gfx/default/16x16_check_update.png"
    refresh->setText(QString::null);
    refresh->setIcon(threedeerefresh);
    connect(refresh, &QPushButton::clicked, this, &GroupBoxPositionalAudioStatus::onRefreshStatus);
    layout->addWidget(refresh, 0, 2, Qt::AlignCenter);

    icon = get_icon_label();
    QPixmap selfthreedeepixmap(":/icons/3d_sound_me.png");  //"gfx/default/20x20_3d_sound_me.png"
    icon->setPixmap(selfthreedeepixmap);
    layout->addWidget(icon, 1, 0, Qt::AlignCenter);

    m_status_self = new QLabel(this);
    layout->addWidget(m_status_self, 1, 1, Qt::AlignLeft);

    onRefreshStatus();
}

void GroupBoxPositionalAudioStatus::UpdateUIStatusSelfName(QString val)
{
    if (val == m_name)
        return;

    m_name = val;

    if (!m_game.isEmpty())
        m_status_self->setText(QString("playing %1 as %2").arg(m_game, m_name));
    else
        m_status_self->clear();
}

void GroupBoxPositionalAudioStatus::UpdateUIStatusSelfGame(QString val)
{
    if (val == m_game)
        return;

    m_game = val;
    if (!m_name.isEmpty())
        m_status_self->setText(QString("playing %1 as %2").arg(m_game, m_name));
    else
        m_status_self->clear();
}

void GroupBoxPositionalAudioStatus::onRefreshStatus()
{
    bool enabled;
    bool isOk = TSSettings::instance()->Is3DSoundEnabled(enabled);
    if (!isOk)
        m_status_settings_threedee->setText(TSSettings::instance()->GetLastError().driverText());
    else
    {
        if (!enabled)
        {
            m_status_settings_threedee->setStyleSheet("QLabel { color : red; }");
            m_status_settings_threedee->setText(
            tr("3D Sound is disabled. You need to enable it in TeamSpeak Options -> Playback Options -> 3D "
               "Sound to enable the audio processing."));
        }
        else
        {
            m_status_settings_threedee->setStyleSheet("QLabel { color : black; }");
            m_status_settings_threedee->setText(tr("3D Sound is enabled."));
        }
    }
}
