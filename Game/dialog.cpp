#include "dialog.h"
#include "ui_dialog.h"

Dialog::Dialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Dialog)
{

    setFixedSize(260,170);
    ui->setupUi(this);
}

Dialog::~Dialog()
{
    delete ui;
}

void Dialog::on_pushButton_clicked()
{
    MainWindow *p=new MainWindow();
    p->show();
    this->close();
}


void Dialog::on_pushButton_2_clicked()
{
    teacher *t=new teacher();
    t->show();
    this->close();
}

