#pragma once

#include <QtWidgets/QGroupBox>

class QLabel;

class GroupBoxPositionalAudioStatus : public QGroupBox
{
    Q_OBJECT
    
public:
    explicit GroupBoxPositionalAudioStatus(QWidget *parent = nullptr);

public slots:
    void UpdateUIStatusSelfName(QString val);
    void UpdateUIStatusSelfGame(QString val);

    void onRefreshStatus();
    
private:
    QString m_name;
    QString m_game;

    QLabel* m_status_settings_threedee;
    QLabel* m_status_self;
};
