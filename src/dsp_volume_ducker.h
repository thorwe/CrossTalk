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
    float m_attackRate;
    float m_decayRate;

    bool m_gainAdjustment;
    bool m_isDuckBlocked;
};

#endif // DSP_VOLUME_DUCKER_H
