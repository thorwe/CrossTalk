#ifndef GROUPBOX_DUCKING_H
#define GROUPBOX_DUCKING_H

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

#endif // GROUPBOX_DUCKING_H
