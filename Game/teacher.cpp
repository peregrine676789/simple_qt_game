#include "teacher.h"
#include "ui_teacher.h"

teacher::teacher(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::teacher)
{
    ui->setupUi(this);
    setFixedSize(960,640);

}

teacher::~teacher()
{
    delete ui;
}

void teacher::on_pushButton_clicked()
{
    Dialog *d=new Dialog();
    d->show();
    this->close();
}

