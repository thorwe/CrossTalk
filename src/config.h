#ifndef CONFIG_H
#define CONFIG_H

#include <QDialog>

namespace Ui {
class Config;
}

class Config : public QDialog
{
	Q_OBJECT

public:
    explicit Config(QWidget *parent = 0);
    ~Config();

protected slots:
    void onJianjiClicked();
    void onBetaChannelToggled(bool val);
    void onServerEnabledToggled(bool val);
    void onServerPortChanged(int val);

signals:
    void betaChannelToggled(bool);
    void serverEnabledToggled(bool);
    void serverPortChanged(quint16);

private:
    Ui::Config *ui;
};

#endif // CONFIG_H
