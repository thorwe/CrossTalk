#include "groupbox_positionalaudio_status.h"
#include "ui_groupbox_positionalaudio_status.h"

#include "ts_settings_qt.h" // make an exception on this one

GroupBoxPositionalAudioStatus::GroupBoxPositionalAudioStatus(QWidget *parent) :
    QGroupBox(parent),
    ui(new Ui::GroupBoxPositionalAudioStatus)
{
    ui->setupUi(this);
    QPixmap threedeepixmap(":/icons/3d_sound.png"); //"gfx/default/24x24_3d_sound.png" also 16x16
    ui->label_3DSettingStatusIcon->setPixmap(threedeepixmap);
    QPixmap selfthreedeepixmap(":/icons/3d_sound_me.png");   //"gfx/default/20x20_3d_sound_me.png"
    ui->label_SelfStatusIcon->setPixmap(selfthreedeepixmap);
    //20x20_3d_sound_user
    QIcon threedeerefresh(":/icons/check_update.png");    //"gfx/default/16x16_check_update.png"
    ui->pushButton_Refresh->setText(QString::null);
    ui->pushButton_Refresh->setIcon(threedeerefresh);
    connect(ui->pushButton_Refresh,SIGNAL(clicked()),this,SLOT(onRefreshStatus()));

    ui->label_3DSettingStatus->setWordWrap(true);
    ui->label_SelfStatus->clear();
    ui->label_3DSettingStatus->setText("Unknown status.");
    onRefreshStatus();
}

GroupBoxPositionalAudioStatus::~GroupBoxPositionalAudioStatus()
{
    delete ui;
}

void GroupBoxPositionalAudioStatus::UpdateUIStatusSelfName(QString val)
{
    if (val == m_Name)
        return;

    m_Name = val;
    if (!m_Game.isEmpty())
        ui->label_SelfStatus->setText(QString("playing %1 as %2").arg(m_Game).arg(m_Name));
    else
        ui->label_SelfStatus->clear();
}

void GroupBoxPositionalAudioStatus::UpdateUIStatusSelfGame(QString val)
{
    if (val == m_Game)
        return;

    m_Game = val;
    if (!m_Name.isEmpty())
        ui->label_SelfStatus->setText(QString("playing %1 as %2").arg(m_Game).arg(m_Name));
    else
        ui->label_SelfStatus->clear();
}

void GroupBoxPositionalAudioStatus::onRefreshStatus()
{
    bool enabled;
    bool isOk = TSSettings::instance()->Is3DSoundEnabled(enabled);
    if (!isOk)
        ui->label_3DSettingStatus->setText(TSSettings::instance()->GetLastError().driverText());
    else
    {
        if (!enabled)
        {
            ui->label_3DSettingStatus->setStyleSheet("QLabel { color : red; }");
            ui->label_3DSettingStatus->setText("3D Sound is disabled. You need to enable it in Teamspeak Options -> Playback Options -> 3D Sound to enable the audio processing.");
        }
        else
        {
            ui->label_3DSettingStatus->setStyleSheet("QLabel { color : black; }");
            ui->label_3DSettingStatus->setText("3D Sound is enabled.");
        }
    }
}

