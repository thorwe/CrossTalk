#pragma once

#include <QObject>

class SimplePanner : public QObject
{
    Q_OBJECT

    Q_PROPERTY(float panCurrent READ getPanCurrent WRITE setPanCurrent)
    Q_PROPERTY(float panDesired READ getPanDesired WRITE setPanDesired)
    Q_PROPERTY(float panDesiredByManual READ getPanDesiredByManual WRITE setPanDesiredByManual)
    Q_PROPERTY(float panDesiredByPanAdjuster READ getPanDesiredByPanAdjuster WRITE setPanDesiredByPanAdjuster)
    Q_PROPERTY(float apaAttackRate READ getApaAttackRate WRITE setApaAttackRate)
    Q_PROPERTY(float apaDecayRate READ getApaDecayRate WRITE setApaDecayRate)
    Q_PROPERTY(bool panAdjustment READ getPanAdjustment WRITE setPanAdjustment)

public:
    explicit SimplePanner(QObject *parent = 0);
    
    // Properties
    void setPanCurrent(float);
    float getPanCurrent() const;
    void setPanDesired(float);
    float getPanDesired() const;
    void setPanDesiredByManual(float);
    float getPanDesiredByManual() const;
    float getPanDesiredByPanAdjuster() const;
    float getApaAttackRate() const;
    float getApaDecayRate() const;
    bool getPanAdjustment() const;

signals:
    void ApaAttackRateChanged(float);
    void ApaDecayRateChanged(float);
    void ChannelPanAdjusted(float);

public slots:
    void setApaAttackRate(float);
    void setApaDecayRate(float);
    void setPanAdjustment(bool);
    void setPanDesiredByPanAdjuster(float);


    void process(short *samples, int sampleCount, int channels, int leftChannelNr, int rightChannelNr);

private:
    //void process(int sampleCount, short *pleft, short *pright);
    static void process(int nSamples, QList<float> *pleft, QList<float> *pright, float balance);
    unsigned short sampleRate = 48000;

    bool panAdjustment = false;

    // Property Privates
    float panCurrent = 0.0f;                // decibels
    float panDesired = 0.0f;                // decibels
    float panDesiredByManual = 0.0f;        // decibels
    float panDesiredByPanAdjuster = 0.0f;   // decibels

    float apaAttackRate = 20.0f;
    float apaDecayRate = 1.0f;
};
