#pragma once

#include <QGroupBox>
#include <QMap>
#include "definitions_positionalaudio.h"

namespace Ui {
class GroupBoxPositionalAudio;
}

class GroupBoxPositionalAudio : public QGroupBox
{
    Q_OBJECT
    
public:
    explicit GroupBoxPositionalAudio(QWidget *parent = 0);
    ~GroupBoxPositionalAudio();

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
    void UpdateUIServerAdd(QString,QString);
    void UpdateUIServerRemove(QString,QString);
    void UpdateUIServerEnabled(QString,bool);
    void UpdateUIServerBlocked(QString,bool);
    void UpdateUIServerSendInterval(QString,float);
    void UpdateUIServerSendIntervalSilentInc(QString,float);

    void UpdateUIServerSelect(QString);

private:
    Ui::GroupBoxPositionalAudio *ui;
};
