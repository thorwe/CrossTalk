#include "dsp_volume_ducker.h"

DspVolumeDucker::DspVolumeDucker(QObject *parent) :
    m_gainAdjustment(false),
    m_isDuckBlocked(false),
    m_attackRate(120.0f),
    m_decayRate(90.0f)
{
    this->setParent(parent);
}

float DspVolumeDucker::getAttackRate() const
{
    return m_attackRate;
}

void DspVolumeDucker::setAttackRate(float val)
{
    if (m_attackRate != val) {
        m_attackRate = val;
        emit attackRateChanged(val);
    }
}

float DspVolumeDucker::getDecayRate() const
{
    return m_decayRate;
}

void DspVolumeDucker::setDecayRate(float val)
{
    if (m_decayRate != val) {
        m_decayRate = val;
        emit decayRateChanged(val);
    }
}

bool DspVolumeDucker::getGainAdjustment() const
{
    return m_gainAdjustment;
}

void DspVolumeDucker::setGainAdjustment(bool val)
{
    m_gainAdjustment = val;
}

bool DspVolumeDucker::isDuckBlocked() const
{
    return m_isDuckBlocked;
}

void DspVolumeDucker::setDuckBlocked(bool val)
{
    m_isDuckBlocked = val;
}

void DspVolumeDucker::setProcessing(bool val)
{
    if(true==val)
    {
        if (true==getGainAdjustment())
            setGainCurrent(getGainDesired());
        else
            setGainCurrent(VOLUME_0DB);
    }
    else
        setGainCurrent(VOLUME_0DB);

    m_isProcessing = val;
}


// virtual funcs
float DspVolumeDucker::GetFadeStep(int sampleCount)
{
    // compute ducker gain
    float current_gain = getGainCurrent();
    if (isDuckBlocked() || isMuted())
        current_gain = VOLUME_0DB;
    else
    {
        float desired_gain = getGainDesired();
        if ((m_gainAdjustment == true) && (current_gain != desired_gain))   // is attacking / adjusting
        {
            float fade_step_down = (m_attackRate / m_sampleRate) * sampleCount;
            float fade_step_up = (m_decayRate / m_sampleRate) * sampleCount;
            if (current_gain < desired_gain - fade_step_up)
                current_gain += fade_step_up;
            else if (current_gain > desired_gain + fade_step_down)
                current_gain -= fade_step_down;
            else
                current_gain = desired_gain;

        }
        else if ((m_gainAdjustment == false) && (current_gain != VOLUME_0DB))    // is releasing
        {
            float fade_step = (m_decayRate / m_sampleRate) * sampleCount;
            if (current_gain < VOLUME_0DB - fade_step)
                current_gain += fade_step;
            else if (current_gain > VOLUME_0DB + fade_step)
                current_gain -= fade_step;
            else
                current_gain = VOLUME_0DB;
        }
    }
    return current_gain;
}
