#pragma once

// "Make up gain / Normalize" variant

#include <QObject>
#include "dsp_volume.h"

class DspVolumeAGMU : public DspVolume
{
    Q_OBJECT

public:
    explicit DspVolumeAGMU(QObject *parent = 0);

    void process(short *samples, int sampleCount, int channels);
    float GetFadeStep(int sampleCount);
    short GetPeak() const;
    void setPeak(short val);    //Overwrite peak; use for reinitializations with cache values etc.
    float computeGainDesired();

signals:
    void peakChanged(short);

public slots:
    void resetPeak() { m_peak = 0; } // on ui close

private slots:

private:
    float m_rateLouder = 90.0f;
    float m_rateQuieter = 120.0f;

    short m_peak = 0;
};
