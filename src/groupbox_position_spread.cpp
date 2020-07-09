#include "groupbox_position_spread.h"

#include <QtWidgets/QApplication>
#include <QtWidgets/QDial>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QSlider>
#include <QtWidgets/QVBoxLayout>

namespace
{
const auto kHome = QLatin1String("Home");
const auto kWhisper = QLatin1String("Whisper");
const auto kOther = QLatin1String("Other");
}  // namespace

GroupBoxPositionSpread::GroupBoxPositionSpread(QWidget *parent)
    : QGroupBox(parent)
    , m_spinbox(new QDoubleSpinBox(this))
    , m_dial(new QDial(this))
    , m_expert(new QGroupBox(this))
{
    setCheckable(true);
    setTitle(tr("Enabled"));  // ToDo: get client translation

    auto layout = new QVBoxLayout(this);

    m_dial->setWhatsThis(
    "<html><head /><body><p><span style = \"font-size:10pt; text-decoration: underline;\">" +
    tr("Position Spread : Width") + "</span></p><p>" +
    tr("Use low values to keep speakers closer together, increase it to set them further apart.") +
    "</p></body></html>");
    m_dial->setNotchesVisible(true);
    layout->addWidget(m_dial);

    m_spinbox->setSizePolicy(QSizePolicy::Fixed, m_spinbox->sizePolicy().verticalPolicy());
    layout->addWidget(m_spinbox);
    layout->setAlignment(m_spinbox, Qt::AlignHCenter);

    layout->addSpacerItem(new QSpacerItem(20, 40));

    m_expert->setCheckable(true);
    m_expert->setTitle(qApp->translate("captureSoundSetupDialog", "&Advanced Options"));
    auto groupbox_expert_layout = new QVBoxLayout;

    auto add_expert_widget = [this, groupbox_expert_layout](const QString &name, const QString &text,
                                                            void (GroupBoxPositionSpread::*slot)(int val)) {
        auto expert_widget = new QWidget;
        expert_widget->setObjectName(QLatin1String("PositionSpread_ExpertWidget_") + name);
        auto expert_layout = new QVBoxLayout;

        auto label = new QLabel;
        // label->setObjectName(expert_widget->objectName() + "_label");
        label->setText(text);
        expert_layout->addWidget(label);

        auto slider = new QSlider;
        // slider->setObjectName(expert_widget->objectName() + "_slider");
        slider->setWhatsThis(tr("Audio Region : Left, Middle, Right"));
        slider->setMaximum(2);
        slider->setPageStep(1);
        slider->setOrientation(Qt::Orientation::Horizontal);
        slider->setTickPosition(QSlider::TicksAbove);
        slider->setMinimumHeight(40);
        connect(slider, &QSlider::valueChanged, this, slot);
        m_sliders.insert(name, slider);
        expert_layout->addWidget(slider);

        expert_widget->setLayout(expert_layout);
        groupbox_expert_layout->addWidget(expert_widget);
    };

    add_expert_widget(kHome, qApp->translate("HotkeyDialog", "Current Server"),
                      &GroupBoxPositionSpread::ExpertHomeValueChanged);
    add_expert_widget(kWhisper, qApp->translate("WhisperSetup", "Whisper"),
                      &GroupBoxPositionSpread::ExpertWhisperValueChanged);
    add_expert_widget(kOther, qApp->translate("NotificationsSetup", "Other"),
                      &GroupBoxPositionSpread::ExpertOtherValueChanged);

    m_expert->setLayout(groupbox_expert_layout);
    layout->addWidget(m_expert);
    setLayout(layout);

    connect(this, &GroupBoxPositionSpread::toggled, this,
            &GroupBoxPositionSpread::EnabledSet);  // remove this redundancy later?
    connect(m_expert, &QGroupBox::toggled, this, &GroupBoxPositionSpread::ExpertEnabledSet);

    connect(m_dial, &QDial::valueChanged, this, &GroupBoxPositionSpread::onDialValueChanged);
    connect(m_spinbox, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this,
            &GroupBoxPositionSpread::onSpinBoxValueChanged);
}

void GroupBoxPositionSpread::UpdateEnabledSet(bool val)
{
    this->blockSignals(true);
    this->setChecked(val);
    this->blockSignals(false);
}

void GroupBoxPositionSpread::UpdateSpreadWidth(float val)
{
    this->blockSignals(true);
    m_spinbox->setValue(static_cast<double>(val * 100));
    m_dial->setValue(static_cast<int>(val * 100));
    this->blockSignals(false);
}

void GroupBoxPositionSpread::UpdateExpertEnabledSet(bool val)
{
    this->blockSignals(true);
    m_expert->setChecked(val);
    this->blockSignals(false);
}

void GroupBoxPositionSpread::UpdateExpertHomeValueChanged(int val)
{
    this->blockSignals(true);
    if (m_sliders.contains(kHome))
        m_sliders[kHome]->setValue(val);

    this->blockSignals(false);
}

void GroupBoxPositionSpread::UpdateExpertWhisperValueChanged(int val)
{
    this->blockSignals(true);
    if (m_sliders.contains(kWhisper))
        m_sliders[kWhisper]->setValue(val);

    this->blockSignals(false);
}

void GroupBoxPositionSpread::UpdateExpertOtherValueChanged(int val)
{
    this->blockSignals(true);
    if (m_sliders.contains(kOther))
        m_sliders[kOther]->setValue(val);

    this->blockSignals(false);
}

//! Retrieves SPS value changed from dial and forwards it to spinbox
/*!
 * \brief GroupBoxPositionSpread::onDialValueChanged Qt slot to retrieve SPS value changed from dial and
 * forwards it to spinbox \param val dial value
 */
void GroupBoxPositionSpread::onDialValueChanged(int val)
{
    m_spinbox->setValue(static_cast<double>(val));
}

//! Retrieves SPS value changed from spinbox, sets slider if it's not the initial sender, emits
//! SetStereoPositionSpreadValue
/*!
 * \brief GroupBoxPositionSpread::onSliderValueChanged Qt slot to retrieve SPS value changed from spinbox
 * \param val spinbox value
 */
void GroupBoxPositionSpread::onSpinBoxValueChanged(double val)
{
    if (!(m_dial->isSliderDown()))  // loop breaker
        m_dial->setValue(static_cast<int>(val));

    emit ValueChanged(static_cast<float>(val * 0.01));
}
