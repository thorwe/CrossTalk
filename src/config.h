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

protected:
//    bool eventFilter(QObject *object, QEvent *ev);

signals:

private:
//    Translator *loca;
//    QNetworkAccessManager *m_netwManager;
//    void Translate(QObject *obj);
};

#endif // CONFIG_H
