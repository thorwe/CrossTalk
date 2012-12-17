#ifndef PANTALKERS_H
#define PANTALKERS_H

#include <QObject>

#include "public_definitions.h"

#include "talkers.h"
#include "simplepanner.h"

/*enum TALKERS_TYPE {
    TALKERS_TYPE_HOMETAB = 0,
    TALKERS_TYPE_WHISPER,
    TALKERS_TYPE_OTHER,
    TALKERS_TYPE_END
};*/

enum TALKERS_REGION {
    TALKERS_REGION_LEFT = 0,
    TALKERS_REGION_CENTER,
    TALKERS_REGION_RIGHT,
    TALKERS_REGION_END
};

class PanTalkers : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool enabled
               READ isEnabled
               WRITE setEnabled
               NOTIFY enabledSet)
    Q_PROPERTY(float spreadWidth
               READ getSpreadWidth
               WRITE setSpreadWidth
               NOTIFY spreadWidthSet)
public:
    explicit PanTalkers(QObject *parent = 0);

    void onEditPostProcessVoiceDataEvent(uint64 serverConnectionHandlerID, anyID clientID, short* samples, int sampleCount, int channels, const unsigned int* channelSpeakerArray, unsigned int* channelFillMask);

    bool isEnabled() const;
    float getSpreadWidth() const;

    void setHomeId(uint64 serverConnectionHandlerID);
    uint64 homeId() {return m_homeId;}

signals:
    void enabledSet(bool);
    void spreadWidthSet(float);
    void expertModeEnabledSet(bool);
    void regionHomeTabSet(TALKERS_REGION);
    void regionWhisperSet(TALKERS_REGION);
    void regionOtherSet(TALKERS_REGION);

public slots:
    void onTalkStatusChanged(uint64 serverConnectionHandlerID, int status, bool isReceivedWhisper, anyID clientID);
    void setEnabled(bool value);
    void setSpreadWidth(float value);

    void setExpertModeEnabled(bool value);
    void setRegionHomeTab(int talkersRegion);
    void setRegionWhisper(int talkersRegion);
    void setRegionOther(int talkersRegion);

private:
    bool m_enabled;
    float m_spreadWidth;

    TSFunctions *ts;
    Talkers* talkers;
    QList<QString>* TalkerSequence;

    QMap<TALKERS_REGION,QList<QString>* >* TalkerSequences;

    static const bool isPrintEnabled = false;
    void Print(QString inqstr);
//    bool isReported;

    //QMap<uint64,QMap<anyID,SimplePanner*>* >* TalkersPanners;
    QMap<QString,SimplePanner*>* TalkersPanners;


    bool m_ExpertModeEnabled;
    uint64 m_homeId;
    TALKERS_REGION m_RegionHomeTab;
    TALKERS_REGION m_RegionWhisper;
    TALKERS_REGION m_RegionOther;
};

#endif // PANTALKERS_H
