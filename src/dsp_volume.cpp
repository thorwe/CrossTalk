#include "dsp_volume.h"

#include "db.h"

const float GAIN_FADE_RATE = (400.0f);	// Rate to fade at (dB per second)

DspVolume::DspVolume(QObject *parent) :
    QObject(parent),
    m_sampleRate(48000),
    m_gainCurrent(VOLUME_0DB),
    m_gainDesired(VOLUME_0DB),
    m_isProcessing(false)
{
}

// Properties

//! Sets the current gain (dB) either set by user interaction or gain adjustment
/*!
  Intended for internal use
  \param val the current gain (dB)
*/
void DspVolume::setGainCurrent(float val)
{
    if (val != m_gainCurrent)
    {
        m_gainCurrent = val;
        emit gainCurrentChanged(m_gainCurrent);
    }
}

//! Gets the current gain (dB) either set by user interaction or gain adjustment
/*!
  \return the current gain (dB)
*/
float DspVolume::getGainCurrent() const
{
    return m_gainCurrent;
}

//! Sets the desired gain (dB) either set by user interaction or gain adjustment
/*!
  Intended for internal use
  \param val the desired gain (dB)
*/
void DspVolume::setGainDesired(float val)
{
    if (val != m_gainDesired)
    {
        m_gainDesired = val;
        emit gainDesiredChanged(m_gainDesired);
    }
}

//! Gets the desired gain (dB) either set by user interaction or gain adjustment
/*!
  \return the desired gain (dB)
*/
float DspVolume::getGainDesired() const
{
    return m_gainDesired;
}

bool DspVolume::isProcessing() const
{
    return m_isProcessing;
}

void DspVolume::setProcessing(bool val)
{
    m_isProcessing = val;
}

//! Mutes the volume
/*!
 * \brief DspVolume::setMuted
 * \param value To mute or to unmute, that is the question
 */
void DspVolume::setMuted(bool val)
{
    m_muted = val;
}

//! Is the volume muted?
/*!
 * \brief DspVolume::isMuted
 * \return or am I just deaf?
 */
bool DspVolume::isMuted() const
{
    return m_muted;
}

void DspVolume::process(short *samples, int sampleCount, int channels)
{
    sampleCount = sampleCount * channels;
    m_gainCurrent = GetFadeStep(sampleCount);
    doProcess(samples,sampleCount);
}

float DspVolume::GetFadeStep(int sampleCount)
{
    // compute manual gain
    float current_gain = getGainCurrent();
    float desired_gain = getGainDesired();
    if (isMuted()) {
        float fade_step = (GAIN_FADE_RATE / m_sampleRate) * sampleCount;
        if (current_gain < VOLUME_MUTED - fade_step) {
            current_gain += fade_step;
        }
        else if (current_gain > VOLUME_MUTED + fade_step) {
            current_gain -= fade_step;
        }
        else {
            current_gain = VOLUME_MUTED;
        }
    }
    else if (current_gain != desired_gain)
    {
        float fade_step = (GAIN_FADE_RATE / m_sampleRate) * sampleCount;
        if (current_gain < desired_gain - fade_step) {
            current_gain += fade_step;
        }
        else if (current_gain > desired_gain + fade_step) {
            current_gain -= fade_step;
        }
        else {
            current_gain = desired_gain;
        }
    }
    return current_gain;
}

//! Apply volume (no need to care for channels)
void DspVolume::doProcess(short *samples, int sampleCount)
{
    float mix_gain = db2lin_alt2(m_gainCurrent);
    for ( int i_sample=0; i_sample<sampleCount; ++i_sample )
    {
        int temp = samples[i_sample] * mix_gain;
        samples[i_sample] = qBound(-32768, temp, 32767);
    }
}
