#pragma once

#include <QtWidgets/QGroupBox>
#include "definitions_positionalaudio.h"

class QPushButton;
class QSpinBox;
class QDoubleSpinBox;

class GroupBoxPositionalAudio : public QGroupBox
{
    Q_OBJECT
    
public:
    explicit GroupBoxPositionalAudio(QWidget *parent = 0);

public slots:
    void UpdateUIUseCameraSet(bool val);
    void UpdateUIUseAttenuationSet(bool val);
    void UpdateUIDistanceMin(int val);
    void UpdateUIDistanceMax(int val);
    void UpdateUIRollOff(float val);
    void UpdateUIRollOffMax(float val);

    void on_DoubleSpinBoxRollOff_valueChanged(double val);
    void on_DoubleSpinBoxRollOffMax_valueChanged(double val);

signals:
    void cameraSet(bool);
    void attenuationSet(bool);
    void distanceMinChanged(int);
    void distanceMaxChanged(int);
    void rollOffChanged(float);
    void rollOffMaxChanged(float);

    void ServerEnabledSet(QString,bool);
    void ServerSendIntervalChange(QString,float);
    void ServerSendIntervalSilentIncChange(QString,float);
    void ServerBlockSet(QString serverUniqueId,bool);
    void ServerAddButtonClicked();
    void ServerRemoveButtonClicked(QString serverUniqueId);

    // to UI
    void UpdateUIServerAdd(QString, QString);
    void UpdateUIServerRemove(QString, QString);
    void UpdateUIServerEnabled(QString, bool);
    void UpdateUIServerBlocked(QString, bool);
    void UpdateUIServerSendInterval(QString, float);
    void UpdateUIServerSendIntervalSilentInc(QString, float);

    void UpdateUIServerSelect(QString);

private:
    QPushButton* m_avatar;
    QPushButton* m_camera;

    QGroupBox* m_volume;
    QSpinBox* m_distance_min;
    QSpinBox* m_distance_max;

    QDoubleSpinBox* m_rolloff;
    QDoubleSpinBox* m_rolloff_max;
};
