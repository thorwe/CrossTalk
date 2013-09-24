#ifndef DSP_RADIO_H
#define DSP_RADIO_H

#include <QObject>
#include "DspFilters/Dsp.h"

class DspRadio : public QObject
{
    Q_OBJECT

    Q_PROPERTY(bool enabled READ getEnabled NOTIFY enabledSet)
    Q_PROPERTY(double fudge READ getFudge NOTIFY fudgeChanged)
    Q_PROPERTY(double eq_bp_center_frequency READ getBandpassEqCenterFrequency NOTIFY bandpassEqLowFrequencyChanged)
    Q_PROPERTY(double eq_bp_band_width READ getBandpassEqBandWidth NOTIFY bandpassEqHighFrequencyChanged)
    Q_PROPERTY(double rm_modFreq READ getRmModFreq NOTIFY ringModFrequencyChanged)

public:
    explicit DspRadio(QObject *parent = 0);
    
    void Process(short* samples, int sampleCount, int channels);

    void setChannelType(QString name);

    bool getEnabled() const {return m_Enabled;}
    float getFudge() const;
    double getBandpassEqCenterFrequency() const;
    double getBandpassEqBandWidth() const;
    double getRmModFreq() const;

signals:
    void enabledSet(bool);
    void fudgeChanged(double);
    void bandpassEqLowFrequencyChanged(double);
    void bandpassEqHighFrequencyChanged(double);
    void ringModFrequencyChanged(double);

public slots:
    void setEnabled(QString name, bool val);
    void setFudge(QString name, double val);
    void setBandpassEqCenterFrequency(QString name, double val);
    void setBandpassEqBandWidth(QString name, double val);
    void setRmModFreq(QString name, double val);

private:
    void DoProcess(float *samples, int sampleCount, float &volFollow);
    void DoProcessRingMod(float *samples, int sampleCount);

    QString m_ChannelType;

    float m_volFollow;
    float m_volFollow_r;

    bool m_Enabled;
    double m_Fudge;

    Dsp::Filter* f_m;
    Dsp::Filter* f_s;

    //RingMod
    double m_RM_modFreq;
    double m_RM_modAngle;
};

#endif // DSP_RADIO_H
