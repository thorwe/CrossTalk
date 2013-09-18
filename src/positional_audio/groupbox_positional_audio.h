#ifndef GROUPBOX_POSITIONAL_AUDIO_H
#define GROUPBOX_POSITIONAL_AUDIO_H

#include <QGroupBox>
#include <QMap>
#include "definitions_positionalaudio.h"

namespace Ui {
class GroupBoxPositionalAudio;
}

class GroupBoxPositionalAudio : public QGroupBox
{
    Q_OBJECT
    
public:
    explicit GroupBoxPositionalAudio(QWidget *parent = 0);
    ~GroupBoxPositionalAudio();

public slots:
    void UpdateUIUseCameraSet(bool val);

signals:
    void enabledSet(bool);
    void cameraSet(bool);

    void ServerEnabledSet(QString,bool);
    void ServerSendIntervalChange(QString,float);
    void ServerSendIntervalSilentIncChange(QString,float);
    void ServerBlockSet(QString serverUniqueId,bool);
    void ServerAddButtonClicked();
    void ServerRemoveButtonClicked(QString serverUniqueId);

    // to UI
    void UpdateUIServerAdd(QString,QString);
    void UpdateUIServerRemove(QString,QString);
    void UpdateUIServerEnabled(QString,bool);
    void UpdateUIServerBlocked(QString,bool);
    void UpdateUIServerSendInterval(QString,float);
    void UpdateUIServerSendIntervalSilentInc(QString,float);

    void UpdateUIServerSelect(QString);

private:
    Ui::GroupBoxPositionalAudio *ui;
};

#endif // GROUPBOX_POSITIONAL_AUDIO_H
