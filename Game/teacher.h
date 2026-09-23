#ifndef TEACHER_H
#define TEACHER_H

#include <QDialog>
#include"dialog.h"
#include"mainwindow.h"

namespace Ui {
class teacher;
}

class teacher : public QDialog
{
    Q_OBJECT

public:
    explicit teacher(QWidget *parent = nullptr);
    ~teacher();



private slots:
    void on_pushButton_clicked();

private:
    Ui::teacher *ui;
};

#endif // TEACHER_H
