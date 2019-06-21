#pragma once

#include <QtWidgets/QDialog>

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
    void onServerEnabledToggled(bool val);
    void onServerPortChanged(int val);

signals:
    void serverEnabledToggled(bool);
    void serverPortChanged(quint16);

private:
    Ui::Config *ui;
};
