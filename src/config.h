#ifndef CONFIG_H
#define CONFIG_H

#include <QDialog>
#include <QtNetwork/QtNetwork>
#include "ui_config.h"
#include "tsfunctions.h"
#include "banner.h"

class Config : public QDialog, public Ui::Config
{
	Q_OBJECT

public:
    Config();
    ~Config();
    void SetupUi();

protected slots:
	void accept();
	void reject();

    void onGlobalDuckerSliderValueChanged(int);
    void onGlobalDuckerValueChanged(double);

    void onDuckSliderValueChanged(int);
    void onDuckValueChanged(double);
    void onDuckModeSliderValueChanged(int);

    void onSPSDialValueChanged(int);
    void onSPSValueChanged(double);

    void onNetwManagerFinished(QNetworkReply *reply);
    void onPledgieClicked();
    void onJianjiClicked();
    void onCrossTalkClicked();
    void onSnTButtonClicked();

protected:
//    bool eventFilter(QObject *object, QEvent *ev);

signals:
    void SetGlobalDuckerEnabled(bool);
    void SetGlobalDuckerValue(float);

    void SetDuckingEnabled(bool);
    void SetDuckingValue(float);
    void SetDuckingReverse(bool);

    void SetStereoPositionSpreadEnabled(bool);
    void SetStereoPositionSpreadValue(float);
    void SetStereoPositionSpreadExpertEnabled(bool);
    void SetStereoPositionSpreadExpertValueHome(int);
    void SetStereoPositionSpreadExpertValueWhisper(int);
    void SetStereoPositionSpreadExpertValueOther(int);

private:
    TSFunctions *ts;
    QNetworkAccessManager *m_netwManager;
    void Translate(QObject *obj);
};

#endif // CONFIG_H
