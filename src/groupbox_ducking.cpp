#include "groupbox_ducking.h"
#include "ui_groupbox_ducking.h"

GroupBoxDucking::GroupBoxDucking(QWidget *parent) :
    QGroupBox(parent),
    ui(new Ui::GroupBoxDucking)
{
    ui->setupUi(this);
    this->connect(ui->wFader_Duck,SIGNAL(valueChanged(float)),SIGNAL(duckerValueChanged(float)));
}

GroupBoxDucking::~GroupBoxDucking()
{
    delete ui;
}
