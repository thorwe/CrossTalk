#include "dsp_volume_agmu.h"

#include <QVarLengthArray>
#include <qmath.h>

#include "dsp_helpers.h"
#include "db.h"
#include "ts_logging_qt.h"

DspVolumeAGMU::DspVolumeAGMU(QObject *parent)
{
    this->setParent(parent);
}

// Funcs

void DspVolumeAGMU::process(short *samples, int sampleCount, int channels)
{
    sampleCount = sampleCount * channels;
    auto peak = getPeak(samples,sampleCount);
    peak = qMax(m_peak,peak);
    if (peak != m_peak)
    {
//        emit peakChanged(peak);
        m_peak = peak;
        setGainDesired(computeGainDesired());
        //TSLogging::Log(QString("Peak: %1 desired Gain: %2").arg(m_peak).arg(getGainDesired()),LogLevel_DEBUG);
    }
    setGainCurrent(GetFadeStep(sampleCount));
    doProcess(samples, sampleCount);
}

// Compute gain change
float DspVolumeAGMU::GetFadeStep(int sampleCount)
{
    auto current_gain = getGainCurrent();
    auto desired_gain = getGainDesired();
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

short DspVolumeAGMU::GetPeak() const
{
    return m_peak;
}

void DspVolumeAGMU::setPeak(short val)
{
    m_peak = val;
}

float DspVolumeAGMU::computeGainDesired()
{
    return qMin((lin2db(32768.f / m_peak)) -2, 12.0f); // leave some headroom
}
