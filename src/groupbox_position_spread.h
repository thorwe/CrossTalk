#pragma once

#include <QtWidgets/QGroupBox>
#include <QtCore/QPointer>

class QDoubleSpinBox;
class QDial;
class QSlider;

class GroupBoxPositionSpread : public QGroupBox
{
    Q_OBJECT

public:
    explicit GroupBoxPositionSpread(QWidget *parent = nullptr);
    ~GroupBoxPositionSpread() = default;

    // For Settings initialization and updating from other sources of interaction
    void UpdateEnabledSet(bool val);
    void UpdateSpreadWidth(float val);
    void UpdateExpertEnabledSet(bool val);
    void UpdateExpertHomeValueChanged(int val);
    void UpdateExpertWhisperValueChanged(int val);
    void UpdateExpertOtherValueChanged(int val);

signals:
    void EnabledSet(bool);
    void ValueChanged(float);
    void ExpertEnabledSet(bool);
    void ExpertHomeValueChanged(int);
    void ExpertWhisperValueChanged(int);
    void ExpertOtherValueChanged(int);

private slots:
    void onDialValueChanged(int val);
    void onSpinBoxValueChanged(double val);

private:
    QDoubleSpinBox* m_spinbox;
    QDial* m_dial;
    QGroupBox* m_expert;
    QHash<QString, QPointer<QSlider> > m_sliders;
};
