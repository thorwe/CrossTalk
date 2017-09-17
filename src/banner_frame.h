#pragma once

#include <QtWidgets/QFrame>

namespace Ui {
class BannerFrame;
}

class BannerFrame : public QFrame
{
    Q_OBJECT
    
public:
    explicit BannerFrame(QWidget *parent = 0);
    ~BannerFrame();

private slots:
    void onCrossTalkClicked();
    void onJianjiClicked();
private:
    Ui::BannerFrame *ui;
};
