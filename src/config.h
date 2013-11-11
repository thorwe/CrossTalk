#ifndef CONFIG_H
#define CONFIG_H

#include <QDialog>
//#include <QtNetwork/QtNetwork>
#include "ui_config.h"
//#include "translator.h"
//#include "banner.h"

class Config : public QDialog, public Ui::Config
{
	Q_OBJECT

public:
    Config();
    ~Config();
    void SetupUi();

protected slots:
    void onJianjiClicked();
    void onBetaChannelToggled(bool val);
    void onServerEnabledToggled(bool val);
    void onServerPortChanged(int val);

protected:
//    bool eventFilter(QObject *object, QEvent *ev);

signals:
    void betaChannelToggled(bool);
    void serverEnabledToggled(bool);
    void serverPortChanged(quint16);

private:
//    Translator *loca;
//    QNetworkAccessManager *m_netwManager;
//    void Translate(QObject *obj);
};

#endif // CONFIG_H
