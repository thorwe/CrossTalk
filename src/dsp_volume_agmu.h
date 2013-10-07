#ifndef DSP_VOLUME_AGMU_H
#define DSP_VOLUME_AGMU_H

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
    float m_rateLouder;
    float m_rateQuieter;

    short m_peak;
};

#endif // DSP_VOLUME_AGMU_H
