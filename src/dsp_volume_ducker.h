#ifndef DSP_VOLUME_DUCKER_H
#define DSP_VOLUME_DUCKER_H

#include <QObject>
#include "dsp_volume.h"

class DspVolumeDucker : public DspVolume
{
    Q_OBJECT
    Q_PROPERTY(float attackRate READ getAttackRate WRITE setAttackRate)
    Q_PROPERTY(float decayRate READ getDecayRate WRITE setDecayRate)

    Q_PROPERTY(bool gainAdjustment READ getGainAdjustment WRITE setGainAdjustment)
    Q_PROPERTY(bool duckBlocked READ isDuckBlocked WRITE setDuckBlocked)  // is Whispering Blacklisting

public:
    explicit DspVolumeDucker(QObject *parent = 0);
    
    float GetFadeStep(int sampleCount);

    float getAttackRate() const;
    float getDecayRate() const;
    bool getGainAdjustment() const;
    bool isDuckBlocked() const;
    void setDuckBlocked(bool val);
    void setProcessing(bool val);

signals:
    void attackRateChanged(float);
    void decayRateChanged(float);

public slots:
    void setAttackRate(float val);
    void setDecayRate(float val);
    void setGainAdjustment(bool val);

private:
    float m_attackRate = 120.0f;
    float m_decayRate = 90.0f;

    bool m_gainAdjustment = false;
    bool m_isDuckBlocked = false;
};

#endif // DSP_VOLUME_DUCKER_H
