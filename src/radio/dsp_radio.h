#ifndef DSP_RADIO_H
#define DSP_RADIO_H

#include <QObject>
#include "DspFilters/Dsp.h"
#include <memory>

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

    float m_volFollow = 0.0f;
    float m_volFollow_r = 0.0f;

    bool m_Enabled;
    double m_Fudge = 0.0f;

    std::unique_ptr<Dsp::Filter> f_m = std::make_unique< Dsp::SmoothedFilterDesign<Dsp::Butterworth::Design::BandPass <4>, 1, Dsp::DirectFormII> >(1024);
    std::unique_ptr<Dsp::Filter> f_s = std::make_unique< Dsp::SmoothedFilterDesign<Dsp::Butterworth::Design::BandPass <4>, 2, Dsp::DirectFormII> >(1024);

    std::unique_ptr<Dsp::Filter> f_m_o = std::make_unique< Dsp::SmoothedFilterDesign<Dsp::Butterworth::Design::BandPass <4>, 1, Dsp::DirectFormII> >(1024);
    std::unique_ptr<Dsp::Filter> f_s_o = std::make_unique< Dsp::SmoothedFilterDesign<Dsp::Butterworth::Design::BandPass <4>, 2, Dsp::DirectFormII> >(1024);

    //RingMod
    double m_RM_modFreq = 0.0f;
    double m_RM_modAngle = 0.0f;
    double m_RM_modAngle_r = 0.0f;
    double m_RM_mix = 0.0f;

};

#endif // DSP_RADIO_H
