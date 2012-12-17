#ifndef SIMPLE_VOLUME_H
#define SIMPLE_VOLUME_H

#include <QObject>

const float VOLUME_0DB = (0.0f);
const float VOLUME_MUTED = (-200.0f);


class SimpleVolume : public QObject
{
    Q_OBJECT

    Q_PROPERTY(float gainCurrent READ getGainCurrent WRITE setGainCurrent)
    Q_PROPERTY(float gainCurrentManual READ getGainCurrentManual WRITE setGainCurrentManual)
    Q_PROPERTY(float gainCurrentDucker READ getGainCurrentDucker WRITE setGainCurrentDucker)
    Q_PROPERTY(float gainDesired READ getGainDesired WRITE setGainDesired)
    Q_PROPERTY(float gainDesiredByManual READ getGainDesiredByManual WRITE setGainDesiredByManual)
    Q_PROPERTY(float gainDesiredByGainAdjuster READ getGainDesiredByGainAdjuster WRITE setGainDesiredByGainAdjuster)
    Q_PROPERTY(bool muted READ isMuted WRITE setMuted)
    Q_PROPERTY(float agaAttackRate READ getAgaAttackRate WRITE setAgaAttackRate)
    Q_PROPERTY(float agaDecayRate READ getAgaDecayRate WRITE setAgaDecayRate)
    Q_PROPERTY(bool gainAdjustment READ getGainAdjustment WRITE setGainAdjustment)
    Q_PROPERTY(bool duckBlocked READ isDuckBlocked WRITE setDuckBlocked)  // is Whispering Blacklisting
    Q_PROPERTY(bool processing READ isProcessing WRITE setProcessing)  // is Talking

public:
    explicit SimpleVolume(QObject *parent = 0);
    
    // Properties
    void setGainCurrent(float);
    float getGainCurrent() const;
    void setGainCurrentManual(float);
    float getGainCurrentManual() const;
    void setGainCurrentDucker(float);
    float getGainCurrentDucker() const;
    void setGainDesired(float);
    float getGainDesired() const;
    void setGainDesiredByManual(float);
    float getGainDesiredByManual() const;
    float getGainDesiredByGainAdjuster() const;
    void setMuted(bool);
    bool isMuted() const;
    float getAgaAttackRate() const;
    float getAgaDecayRate() const;
    bool getGainAdjustment() const;
    bool isDuckBlocked() const;
    void setDuckBlocked(bool);
    bool isProcessing() const;
    void setProcessing(bool);

signals:
    void AgaAttackRateChanged(float);
    void AgaDecayRateChanged(float);
    void ChannelGainAdjusted(float);

public slots:
    void setAgaAttackRate(float);
    void setAgaDecayRate(float);
    void setGainAdjustment(bool);
    void setGainDesiredByGainAdjuster(float);

    void process(short* samples, int sampleCount);

private:
    unsigned short sampleRate;

    bool gainAdjustment;

    // Property Privates
    float gainCurrent;                  // decibels
    float m_gainCurrentManual;          // decibels
    float m_gainCurrentDucker;          // decibels
    float gainDesired;                  // decibels
    float gainDesiredByManual;          // decibels
    float gainDesiredByGainAdjuster;    // decibels
    bool muted;
    float agaAttackRate;
    float agaDecayRate;
    bool m_isDuckBlocked;
    bool m_isProcessing;
};

#endif // SIMPLE_VOLUME_H
