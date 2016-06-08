#pragma once

#include <QGroupBox>

namespace Ui {
class GroupBoxDucking;
}

class GroupBoxDucking : public QGroupBox
{
    Q_OBJECT

public:
    explicit GroupBoxDucking(QWidget *parent = 0);
    ~GroupBoxDucking();
    Ui::GroupBoxDucking *ui;

signals:
    void duckerValueChanged(float);

private:

};
