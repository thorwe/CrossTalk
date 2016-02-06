#include "groupbox_ducking.h"
#include "ui_groupbox_ducking.h"

GroupBoxDucking::GroupBoxDucking(QWidget *parent) :
    QGroupBox(parent),
    ui(new Ui::GroupBoxDucking)
{
    ui->setupUi(this);
    connect(ui->wFader_Duck, &FaderVertical::valueChanged, this, &GroupBoxDucking::duckerValueChanged);
}

GroupBoxDucking::~GroupBoxDucking()
{
    delete ui;
}
