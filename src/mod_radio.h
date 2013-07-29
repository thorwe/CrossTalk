#ifndef MOD_RADIO_H
#define MOD_RADIO_H

#include <QObject>

#include "module.h"
#include "talkers.h"
#include "dsp_radio.h"

class Radio : public Module, public TalkInterface
{
    Q_OBJECT
    Q_INTERFACES(TalkInterface)

    Q_PROPERTY(uint64 homeId READ homeId WRITE setHomeId)

    Q_PROPERTY(bool enabled_HomeTab READ getEnabledHomeTab WRITE setEnabledHomeTab NOTIFY EnabledHomeTabSet)
    Q_PROPERTY(bool enabled_Whisper READ getEnabledWhisper WRITE setEnabledWhisper NOTIFY EnabledWhisperSet)
    Q_PROPERTY(bool enabled_Other READ getEnabledOther WRITE setEnabledOther NOTIFY EnabledOtherSet)

    Q_PROPERTY(double fudge_HomeTab READ getFudgeHomeTab WRITE setFudgeHomeTab NOTIFY FudgeHomeTabSet)
    Q_PROPERTY(double fudge_Whisper READ getFudgeWhisper WRITE setFudgeWhisper NOTIFY FudgeWhisperSet)
    Q_PROPERTY(double fudge_Other READ getFudgeOther WRITE setFudgeOther NOTIFY FudgeOtherSet)

    Q_PROPERTY(double centerFrequency_HomeTab READ getCenterFrequencyHomeTab NOTIFY CenterFrequencyHomeTabSet)  //WRITE setCenterFrequencyHomeTab
    Q_PROPERTY(double centerFrequency_Whisper READ getCenterFrequencyWhisper NOTIFY CenterFrequencyWhisperSet)  //WRITE setCenterFrequencyWhisper
    Q_PROPERTY(double centerFrequency_Other READ getCenterFrequencyOther NOTIFY CenterFrequencyOtherSet)        //WRITE setCenterFrequencyOther

    Q_PROPERTY(double bandWidth_HomeTab READ getBandWidthHomeTab NOTIFY BandWidthHomeTabSet)    //WRITE setBandWidthHomeTab
    Q_PROPERTY(double bandWidth_Whisper READ getBandWidthWhisper NOTIFY BandWidthWhisperSet)    //WRITE setBandWidthWhisper
    Q_PROPERTY(double bandWidth_Other READ getBandWidthOther NOTIFY BandWidthOtherSet)          //WRITE setBandWidthOther

    Q_PROPERTY(double lowFrequency_HomeTab READ getLowFrequencyHomeTab WRITE setLowFrequencyHomeTab NOTIFY LowFrequencyHomeTabSet)
    Q_PROPERTY(double lowFrequency_Whisper READ getLowFrequencyWhisper WRITE setLowFrequencyWhisper NOTIFY LowFrequencyWhisperSet)
    Q_PROPERTY(double lowFrequency_Other READ getLowFrequencyOther WRITE setLowFrequencyOther NOTIFY LowFrequencyOtherSet)

    Q_PROPERTY(double highFrequency_HomeTab READ getHighFrequencyHomeTab WRITE setHighFrequencyHomeTab NOTIFY HighFrequencyHomeTabSet)
    Q_PROPERTY(double highFrequency_Whisper READ getHighFrequencyWhisper WRITE setHighFrequencyWhisper NOTIFY HighFrequencyWhisperSet)
    Q_PROPERTY(double highFrequency_Other READ getHighFrequencyOther WRITE setHighFrequencyOther NOTIFY HighFrequencyOtherSet)

public:
    explicit Radio(QObject *parent = 0);
    
    bool onTalkStatusChanged(uint64 serverConnectionHandlerID, int status, bool isReceivedWhisper, anyID clientID, bool isMe);

    void setHomeId(uint64 serverConnectionHandlerID);
    uint64 homeId() {return m_homeId;}

    bool getEnabledHomeTab() {return m_enabled_HomeTab;}
    bool getEnabledWhisper() {return m_enabled_Whisper;}
    bool getEnabledOther() {return m_enabled_Other;}

    double getFudgeHomeTab() {return m_fudge_HomeTab;}
    double getFudgeWhisper() {return m_fudge_Whisper;}
    double getFudgeOther() {return m_fudge_Other;}

    double getCenterFrequencyHomeTab() {return m_lowFrequency_HomeTab + (getBandWidthHomeTab() / 2.0);}
    double getCenterFrequencyWhisper() {return m_lowFrequency_Whisper + (getBandWidthWhisper() / 2.0);}
    double getCenterFrequencyOther() {return m_lowFrequency_Other + (getBandWidthOther() / 2.0);}

    double getBandWidthHomeTab() {return m_highFrequency_HomeTab - m_lowFrequency_HomeTab;}
    double getBandWidthWhisper() {return m_highFrequency_Whisper - m_lowFrequency_Whisper;}
    double getBandWidthOther() {return m_highFrequency_Other - m_lowFrequency_Other;}

    double getLowFrequencyHomeTab() {return m_lowFrequency_HomeTab;}
    double getLowFrequencyWhisper() {return m_lowFrequency_Whisper;}
    double getLowFrequencyOther() {return m_lowFrequency_Other;}

    double getHighFrequencyHomeTab() {return m_highFrequency_HomeTab;}
    double getHighFrequencyWhisper() {return m_highFrequency_Whisper;}
    double getHighFrequencyOther() {return m_highFrequency_Other;}

    bool isClientBlacklisted(uint64 serverConnectionHandlerID, anyID clientID);

    void onEditPlaybackVoiceDataEvent(uint64 serverConnectionHandlerID, anyID clientID, short* samples, int sampleCount, int channels);

signals:
    void EnabledHomeTabSet(bool);
    void EnabledWhisperSet(bool);
    void EnabledOtherSet(bool);

    void FudgeHomeTabSet(double);
    void FudgeWhisperSet(double);
    void FudgeOtherSet(double);

    void CenterFrequencyHomeTabSet(double);
    void CenterFrequencyWhisperSet(double);
    void CenterFrequencyOtherSet(double);

    void BandWidthHomeTabSet(double);
    void BandWidthWhisperSet(double);
    void BandWidthOtherSet(double);

    void LowFrequencyHomeTabSet(double);
    void LowFrequencyWhisperSet(double);
    void LowFrequencyOtherSet(double);

    void HighFrequencyHomeTabSet(double);
    void HighFrequencyWhisperSet(double);
    void HighFrequencyOtherSet(double);

public slots:
    void setEnabledHomeTab(bool val);
    void setEnabledWhisper(bool val);
    void setEnabledOther(bool val);

    void setFudgeHomeTab(double val);
    void setFudgeWhisper(double val);
    void setFudgeOther(double val);

    void setLowFrequencyHomeTab(double val);
    void setLowFrequencyWhisper(double val);
    void setLowFrequencyOther(double val);

    void setHighFrequencyHomeTab(double val);
    void setHighFrequencyWhisper(double val);
    void setHighFrequencyOther(double val);

    void ToggleClientBlacklisted(uint64 serverConnectionHandlerID, anyID clientID);

    void saveSettings(int r);

private:
    uint64 m_homeId;
    Talkers* talkers;

    QMap<uint64,QMap<anyID,DspRadio*>* >* TalkersDspRadios;

    bool m_enabled_HomeTab;
    bool m_enabled_Whisper;
    bool m_enabled_Other;

    double m_fudge_HomeTab;
    double m_fudge_Whisper;
    double m_fudge_Other;

    double m_lowFrequency_HomeTab;
    double m_lowFrequency_Whisper;
    double m_lowFrequency_Other;

    double m_highFrequency_HomeTab;
    double m_highFrequency_Whisper;
    double m_highFrequency_Other;

    // QMultiMap is reported to be faster than QMultiHash until up to 10 entries in 4.x, oh I dunno
    QMultiMap<uint64,uint64> m_ClientBlacklist;

protected:
    void onRunningStateChanged(bool value);
};

#endif // MOD_RADIO_H
