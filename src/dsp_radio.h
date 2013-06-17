#ifndef DSP_RADIO_H
#define DSP_RADIO_H

#include <QObject>
#include "DspFilters/Dsp.h"

class DspRadio : public QObject
{
    Q_OBJECT

    Q_PROPERTY(bool enabled READ getEnabled WRITE setEnabled NOTIFY enabledSet)
    Q_PROPERTY(double fudge READ getFudge WRITE setFudge NOTIFY fudgeChanged)
    Q_PROPERTY(double eq_bp_center_frequency READ getBandpassEqCenterFrequency WRITE setBandpassEqCenterFrequency)
    Q_PROPERTY(double eq_bp_band_width READ getBandpassEqBandWidth WRITE setBandpassEqBandWidth)

public:
    explicit DspRadio(QObject *parent = 0);
    
    void Process(short* samples, int sampleCount, int channels);

    bool getEnabled() const {return m_Enabled;}
    float getFudge() const;
    double getBandpassEqCenterFrequency() const;
    double getBandpassEqBandWidth() const;

signals:
    void enabledSet(bool);
    void fudgeChanged(double);

public slots:
    void setEnabled(bool val);
    void setFudge(double val);
    void setBandpassEqCenterFrequency(double val);
    void setBandpassEqBandWidth(double val);

private:
    void DoProcess(float *samples, int sampleCount, float &volFollow);
    float m_volFollow;
    float m_volFollow_r;

    bool m_Enabled;
    double m_Fudge;

    Dsp::Filter* f_m;
    Dsp::Filter* f_s;
};

#endif // DSP_RADIO_H
