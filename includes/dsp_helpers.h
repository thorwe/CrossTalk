#ifndef DSP_HELPERS_H
#define DSP_HELPERS_H

#include <qmath.h>

// Peak
static inline float getPeak(float *samples, int sampleCount)
{
    float val = 0.0f;
    for(int i=0; i<sampleCount; ++i)
        val = qMax(qAbs(samples[i]),val);

    return val;
}

// Peak unsigned 16bit
static inline short getPeak(short *samples, int sampleCount)
{
    short val = 0.0f;
    for(int i=0; i<sampleCount; ++i)
        val = qMax(qAbs(samples[i]),val);

    return val;
}

// Average Power (RMS)
static inline float getRMS(float *samples, int sampleCount)
{
    float val = 0.0f;
    for(int i=0; i<sampleCount; ++i)
    {
        float sample = samples[i];
        val += sample * sample; // I hear pow is slower
    }
    return qSqrt( val / sampleCount );
}

// Both
static inline float getPeakRMS(float *samples, int sampleCount, float& rms)
{
    float peak = 0.0f;
    rms = 0.0f;
    for(int i=0; i<sampleCount; ++i)
    {
        float sample = samples[i];
        peak = qMax(qAbs(sample),peak);
        rms += sample * sample; //I hear pow is slower
    }
    rms = qSqrt( rms / sampleCount );
    return peak;
}

#endif // DSP_HELPERS_H
