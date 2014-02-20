#ifndef DSP_RADIO_H
#define DSP_RADIO_H

#include <QObject>
#include "DspFilters/Dsp.h"

class DspRadio : public QObject
{
    Q_OBJECT

    Q_PROPERTY(bool enabled READ getEnabled NOTIFY enabledSet)
    Q_PROPERTY(double fudge READ getFudge NOTIFY fudgeChanged)
    Q_PROPERTY(double eq_bp_center_frequency READ getBandpassEqInCenterFrequency NOTIFY bandpassEqInCenterFrequencyChanged)
    Q_PROPERTY(double eq_bp_band_width READ getBandpassEqInBandWidth NOTIFY bandpassEqInBandWidthChanged)
    Q_PROPERTY(double rm_modFreq READ getRmModFreq NOTIFY ringModFrequencyChanged)

public:
    explicit DspRadio(QObject *parent = 0);
    
    void Process(short* samples, int sampleCount, int channels);

    void setChannelType(QString name);

    bool getEnabled() const {return m_Enabled;}
    float getFudge() const;
    double getBandpassEqInCenterFrequency() const;
    double getBandpassEqInBandWidth() const;
    double getRmModFreq() const;
    double getBandpassEqOutCenterFrequency() const;
    double getBandpassEqOutBandWidth() const;

signals:
    void enabledSet(bool);
    void fudgeChanged(double);
    void bandpassEqInCenterFrequencyChanged(double);
    void bandpassEqInBandWidthChanged(double);
    void ringModFrequencyChanged(double);
    void ringModMixChanged(double);
    void bandpassEqOutCenterFrequencyChanged(double);
    void bandpassEqOutBandWidthChanged(double);

public slots:
    void setEnabled(QString name, bool val);
    void setFudge(QString name, double val);
    void setBandpassEqInCenterFrequency(QString name, double val);
    void setBandpassEqInBandWidth(QString name, double val);
    void setRmModFreq(QString name, double val);
    void setRmMix(QString name, double val);
    void setBandpassEqOutCenterFrequency(QString name, double val);
    void setBandpassEqOutBandWidth(QString name, double val);

private:
    void DoProcess(float *samples, int sampleCount, float &volFollow);
    void DoProcessRingMod(float *samples, int sampleCount, double &modAngle);

    QString m_ChannelType;

    float m_volFollow;
    float m_volFollow_r;

    bool m_Enabled;
    double m_Fudge;

    Dsp::Filter* f_m;
    Dsp::Filter* f_s;

    Dsp::Filter* f_m_o;
    Dsp::Filter* f_s_o;

    //RingMod
    double m_RM_modFreq;
    double m_RM_modAngle;
    double m_RM_modAngle_r;
    double m_RM_mix;

};

#endif // DSP_RADIO_H
