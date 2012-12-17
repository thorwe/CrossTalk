#include "simple_volume.h"

#include "db.h"

const float GAIN_FADE_RATE = (400.0f);	// Rate to fade at (dB per second)

SimpleVolume::SimpleVolume(QObject *parent) :
    QObject(parent),
    sampleRate(48000),
    gainCurrent(VOLUME_0DB),
    m_gainCurrentManual(VOLUME_0DB),
    m_gainCurrentDucker(VOLUME_0DB),
    m_isDuckBlocked(false),
    m_isProcessing(false),
    gainDesired(VOLUME_0DB),
    gainDesiredByManual(VOLUME_0DB),
    gainDesiredByGainAdjuster(VOLUME_0DB),
    muted(false),
    gainAdjustment(false),
    agaAttackRate(120.0f),
    agaDecayRate(90.0f)
{
}

// Properties

//! Sets the current gain (dB) either set by user interaction or gain adjustment
/*!
  Intended for internal use
  \param value the current gain (dB)
*/
void SimpleVolume::setGainCurrent(float value)
{
    gainCurrent = value;
}

//! Gets the current gain (dB) either set by user interaction or gain adjustment
/*!
  \return the current gain (dB)
*/
float SimpleVolume::getGainCurrent() const
{
    return gainCurrent;
}

void SimpleVolume::setGainCurrentManual(float val)
{
    m_gainCurrentManual = val;
}

float SimpleVolume::getGainCurrentManual() const
{
    return m_gainCurrentManual;
}

void SimpleVolume::setGainCurrentDucker(float val)
{
    m_gainCurrentDucker = val;
}

float SimpleVolume::getGainCurrentDucker() const
{
    return m_gainCurrentDucker;
}

//! Sets the desired gain (dB) either set by user interaction or gain adjustment
/*!
  Intended for internal use
  \param value the desired gain (dB)
*/
void SimpleVolume::setGainDesired(float value)
{
    gainDesired = value;
}

//! Gets the desired gain (dB) either set by user interaction or gain adjustment
/*!
  \return the desired gain (dB)
*/
float SimpleVolume::getGainDesired() const
{
    return gainDesired;
}

//! Sets the desired gain (dB) of a user interaction
/*!
  Intended for internal use
  \param value the desired gain of a user interaction (dB)
*/
void SimpleVolume::setGainDesiredByManual(float value)
{
    gainDesiredByManual = value;
    setGainDesired(value);
}

//! Gets the desired gain (dB) set by the last user interaction
/*!
  \return the desired gain (dB) of the last user interaction
*/
float SimpleVolume::getGainDesiredByManual() const
{
    return gainDesiredByManual;
}

//! Sets the desired gain (dB) of an automatic gain adjustment
/*!
  Intended for internal use
  \param value the desired gain of an automatic gain adjustment (dB)
*/
void SimpleVolume::setGainDesiredByGainAdjuster(float value)
{
    gainDesiredByGainAdjuster = value;
    setGainDesired(value);
}

//! Gets the desired gain (dB) set by the last automatic gain adjustment
/*!
  \return the desired gain (dB) of the automatic gain adjustment
*/
float SimpleVolume::getGainDesiredByGainAdjuster() const
{
    return gainDesiredByGainAdjuster;
}

//! Mutes the volume
/*!
 * \brief SimpleVolume::setMuted
 * \param value To mute or to unmute, that is the question
 */
void SimpleVolume::setMuted(bool value)
{
    muted = value;
}

//! Is the volume muted?
/*!
 * \brief SimpleVolume::isMuted
 * \return or am I just deaf?
 */
bool SimpleVolume::isMuted() const
{
    return muted;
}

float SimpleVolume::getAgaAttackRate() const
{
    return agaAttackRate;
}

float SimpleVolume::getAgaDecayRate() const
{
    return agaDecayRate;
}

bool SimpleVolume::getGainAdjustment() const
{
    return gainAdjustment;
}

bool SimpleVolume::isDuckBlocked() const
{
    return m_isDuckBlocked;
}

void SimpleVolume::setDuckBlocked(bool val)
{
    m_isDuckBlocked = val;
}

bool SimpleVolume::isProcessing() const
{
    return m_isProcessing;
}

void SimpleVolume::setProcessing(bool val)
{
    if(true==val)
    {
        if (true==getGainAdjustment())
            setGainCurrentDucker(getGainDesiredByGainAdjuster());
        else
            setGainCurrentDucker(VOLUME_0DB);
    }
    else
        setGainCurrentDucker(VOLUME_0DB);

    m_isProcessing = val;
}

// Slots

void SimpleVolume::setAgaAttackRate(float value)
{
    if (agaAttackRate != value) {
        agaAttackRate = value;
        emit AgaAttackRateChanged(value);
    }

}

void SimpleVolume::setAgaDecayRate(float value)
{
    if (agaDecayRate != value) {
        agaDecayRate = value;
        emit AgaDecayRateChanged(value);
    }
}

void SimpleVolume::setGainAdjustment(bool value)
{
    gainAdjustment = value;
    if ((value == false) && (isProcessing()))
        printf("Should release.\n");
    //printf("SimpleVolume:setGainAdjustment: %s\n",(gainAdjustment)?"true":"false");
}

//! Process samples delivered by onEditPlaybackVoiceDataEvent
/*!
 * \brief SimpleVolume::process
 * \param samples Pointer to the voice data (signed 16 bit @ 48KHz)
 * \param sampleCount Number of samples the "samples" variable points to.
 */
void SimpleVolume::process(short *samples, int sampleCount)
{
    // compute manual gain
    float currentGainManual = getGainCurrentManual();
    float desiredGainManual = getGainDesiredByManual();
    if (isMuted()) {
        float fade_step = (GAIN_FADE_RATE / sampleRate) * sampleCount;
        if (currentGainManual < VOLUME_MUTED - fade_step) {
            currentGainManual += fade_step;
        }
        else if (currentGainManual > VOLUME_MUTED + fade_step) {
            currentGainManual -= fade_step;
        }
        else {
            currentGainManual = VOLUME_MUTED;
        }
    }
    else if (currentGainManual != desiredGainManual)
    {
        float fade_step = (GAIN_FADE_RATE / sampleRate) * sampleCount;
        if (currentGainManual < desiredGainManual - fade_step) {
            currentGainManual += fade_step;
        }
        else if (currentGainManual > desiredGainManual + fade_step) {
            currentGainManual -= fade_step;
        }
        else {
            currentGainManual = desiredGainManual;
        }
    }
    setGainCurrentManual(currentGainManual);

    // compute ducker gain
    float currentGainDucker = getGainCurrentDucker();
    if (isDuckBlocked() || isMuted())
        currentGainDucker = VOLUME_0DB;
    else
    {
        float desiredGainDucker = getGainDesiredByGainAdjuster();
        if ((gainAdjustment == true) && (currentGainDucker != desiredGainDucker))   // is attacking / adjusting
        {
            float fade_step_down = (agaAttackRate / sampleRate) * sampleCount;
            float fade_step_up = (agaDecayRate / sampleRate) * sampleCount;
            if (currentGainDucker < desiredGainDucker - fade_step_up)
                currentGainDucker += fade_step_up;
            else if (currentGainDucker > desiredGainDucker + fade_step_down)
                currentGainDucker -= fade_step_down;
            else
                currentGainDucker = desiredGainDucker;

//            if (getGainCurrentDucker() != currentGainDucker)
//                printf("Attacking: %f\n",currentGainDucker);

        }
        else if ((gainAdjustment == false) && (currentGainDucker != VOLUME_0DB))    // is releasing
        {
            float fade_step = (agaDecayRate / sampleRate) * sampleCount;
            if (currentGainDucker < VOLUME_0DB - fade_step)
                currentGainDucker += fade_step;
            else if (currentGainDucker > VOLUME_0DB + fade_step)
                currentGainDucker -= fade_step;
            else
                currentGainDucker = VOLUME_0DB;

//            if (getGainCurrentDucker() != currentGainDucker)
//                printf("Releasing: %f\n",currentGainDucker);
        }
        //emit ChannelGainAdjusted(currentGainDucker);
    }
    setGainCurrentDucker(currentGainDucker);

    //    float desiredGain = getGainDesired();
    float currentGain = currentGainManual + currentGainDucker;

//    if (currentGain != getGainCurrent())
//    {
//        printf("Gain: %f\n", currentGain);
//        emit ChannelGainAdjusted(currentGain);
//    }

    setGainCurrent(currentGain);
    // gain computation complete

    // audio processing
    float mix_gain = db2lin_alt2(currentGain);
    for ( int i_sample=0; i_sample<sampleCount; ++i_sample ) {
        samples[i_sample] *= mix_gain;
    }
}
