#ifndef CONFIG_POSITIONALAUDIO_H
#define CONFIG_POSITIONALAUDIO_H

#include <QDialog>
#include "definitions_positionalaudio.h"

namespace Ui {
class ConfigPositionalAudio;
}

class ConfigPositionalAudio : public QDialog
{
    Q_OBJECT
    
public:
    explicit ConfigPositionalAudio(QWidget *parent = 0);
    ~ConfigPositionalAudio();

signals:
    // from UI
    void EnabledSet(bool);
    void CameraSet(bool);

    void ServerEnabledSet(QString,bool);
    void ServerSendIntervalChange(QString,float);
    void ServerSendIntervalSilentIncChange(QString,float);
    void ServerBlockSet(QString serverUniqueId,bool);
    void ServerAddButtonClicked();
    void ServerRemoveButtonClicked(QString serverUniqueId);

    // to UI
    void UpdateUIEnabledSet(bool);
    void UpdateUIUseCameraSet(bool);

    void UpdateUIServerAdd(QString,QString);
    void UpdateUIServerRemove(QString,QString);
    void UpdateUIServerEnabled(QString,bool);
    void UpdateUIServerBlocked(QString,bool);
    void UpdateUIServerSendInterval(QString,float);
    void UpdateUIServerSendIntervalSilentInc(QString,float);

    void UpdateUIServerSelect(QString);

    void UpdateUIStatusSelfName(QString);
    void UpdateUIStatusSelfGame(QString);
    
private:
    Ui::ConfigPositionalAudio *ui;
};

#endif // CONFIG_POSITIONALAUDIO_H
