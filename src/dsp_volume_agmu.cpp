#include "dsp_volume_agmu.h"

#include <QVarLengthArray>
#include <qmath.h>

#include "dsp_helpers.h"
#include "db.h"

DspVolumeAGMU::DspVolumeAGMU(QObject *parent) :
    m_rateQuieter(120.0f),
    m_rateLouder(90.0f),
    m_peak(0)
{
    this->setParent(parent);
}

// Funcs

void DspVolumeAGMU::process(short *samples, int sampleCount, int channels)
{
    sampleCount = sampleCount * channels;
    short peak = getPeak(samples,sampleCount);
    peak = qMax(m_peak,peak);
    if (peak != m_peak)
        emit peakChanged(peak);

    m_peak = peak;

    setGainDesired(lin2db(32000.f / m_peak));   //32768.f leave some headroom

    setGainCurrent(GetFadeStep(sampleCount));
    doProcess(samples, sampleCount);
}

// Compute gain change
float DspVolumeAGMU::GetFadeStep(int sampleCount)
{
    float current_gain = getGainCurrent();
    float desired_gain = getGainDesired();
    if (current_gain != desired_gain)
    {
        float fade_step_down = (m_rateQuieter / m_sampleRate) * sampleCount;
        float fade_step_up = (m_rateLouder / m_sampleRate) * sampleCount;
        if (current_gain < desired_gain - fade_step_up)
            current_gain += fade_step_up;
        else if (current_gain > desired_gain + fade_step_down)
            current_gain -= fade_step_down;
        else
            current_gain = desired_gain;

    }

    return current_gain;
}
