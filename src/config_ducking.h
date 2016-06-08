#pragma once

#include <QDialog>
#include <QGroupBox>
#include <QCheckBox>
#include <QRadioButton>

#include "fader_vertical.h"

namespace Ui {
class ConfigDucking;
}

class ConfigDucking : public QDialog
{
    Q_OBJECT
    
public:
    explicit ConfigDucking(QWidget *parent = 0);
    ~ConfigDucking();

    // For Settings initialization and updating from other sources of interaction
    void UpdateGlobalDuckerEnabled(bool);
    void UpdateGlobalDuckerValue(float);
    void UpdateChannelDuckerEnabled(bool);
    void UpdateChannelDuckerValue(float);
    void UpdateChannelDuckerReverse(bool);
    void UpdateChannelDuckerDuckPSEnabled(bool);

signals:
    void globalDuckerEnabledSet(bool);
    void globalDuckerValueChanged(float);

    void channelDuckerEnabledSet(bool);
    void channelDuckerValueChanged(float);
    void channelDuckerReverseSet(bool);
    void channelDuckerDuckPSEnabledSet(bool);

private slots:
    void onCRadioTargetCurrentToggled(bool val);
    void onCRadioTargetOtherToggled(bool val);

private:
    Ui::ConfigDucking *ui;
    QGroupBox* g;
    QGroupBox* c;
    FaderVertical* gFader;
    FaderVertical* cFader;

    QRadioButton* cRadioTargetCurrent;
    QRadioButton* cRadioTargetOther;
    QCheckBox* cPS;
};
