#ifndef GROUPBOX_POSITIONALAUDIO_STATUS_H
#define GROUPBOX_POSITIONALAUDIO_STATUS_H

#include <QGroupBox>

namespace Ui {
class GroupBoxPositionalAudioStatus;
}

class GroupBoxPositionalAudioStatus : public QGroupBox
{
    Q_OBJECT
    
public:
    explicit GroupBoxPositionalAudioStatus(QWidget *parent = 0);
    ~GroupBoxPositionalAudioStatus();

public slots:
    void UpdateUIStatusSelfName(QString val);
    void UpdateUIStatusSelfGame(QString val);

    void onRefreshStatus();
    
private:
    Ui::GroupBoxPositionalAudioStatus *ui;
    QString m_Name;
    QString m_Game;
};

#endif // GROUPBOX_POSITIONALAUDIO_STATUS_H
