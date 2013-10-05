#ifndef DSP_VOLUME_H
#define DSP_VOLUME_H

#include <QObject>

const float VOLUME_0DB = (0.0f);
const float VOLUME_MUTED = (-200.0f);

class DspVolume : public QObject
{
    Q_OBJECT
    Q_PROPERTY(float gainCurrent READ getGainCurrent WRITE setGainCurrent NOTIFY gainCurrentChanged)
    Q_PROPERTY(float gainDesired READ getGainDesired WRITE setGainDesired NOTIFY gainDesiredChanged)
    Q_PROPERTY(bool processing READ isProcessing WRITE setProcessing)  // is Talking
    Q_PROPERTY(bool muted READ isMuted WRITE setMuted)

public:
    explicit DspVolume(QObject *parent = 0);

    // Properties
    void setGainCurrent(float val);
    float getGainCurrent() const;
    void setGainDesired(float val);
    float getGainDesired() const;
    bool isProcessing() const;
    virtual void setProcessing(bool val);
    void setMuted(bool val);
    bool isMuted() const;

    virtual void process(short* samples, int sampleCount, int channels);
    virtual float GetFadeStep(int sampleCount);

signals:
    void gainCurrentChanged(float);
    void gainDesiredChanged(float);

public slots:
    
protected:
    unsigned short m_sampleRate;
    void doProcess(short *samples, int sampleCount);
    bool m_isProcessing;

private:
    float m_gainCurrent;    // decibels
    float m_gainDesired;    // decibels
    bool m_muted;
};

#endif // DSP_VOLUME_H
