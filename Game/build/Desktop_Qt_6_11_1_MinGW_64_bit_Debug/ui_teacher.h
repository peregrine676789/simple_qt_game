/********************************************************************************
** Form generated from reading UI file 'teacher.ui'
**
** Created by: Qt User Interface Compiler version 6.11.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_TEACHER_H
#define UI_TEACHER_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>

QT_BEGIN_NAMESPACE

class Ui_teacher
{
public:
    QLineEdit *lineEdit;
    QLineEdit *lineEdit_2;
    QLineEdit *lineEdit_3;
    QPushButton *pushButton_2;

    void setupUi(QDialog *teacher)
    {
        if (teacher->objectName().isEmpty())
            teacher->setObjectName("teacher");
        teacher->resize(500, 340);
        lineEdit = new QLineEdit(teacher);
        lineEdit->setObjectName("lineEdit");
        lineEdit->setGeometry(QRect(30, 80, 241, 20));
        lineEdit_2 = new QLineEdit(teacher);
        lineEdit_2->setObjectName("lineEdit_2");
        lineEdit_2->setGeometry(QRect(30, 140, 241, 20));
        lineEdit_3 = new QLineEdit(teacher);
        lineEdit_3->setObjectName("lineEdit_3");
        lineEdit_3->setGeometry(QRect(30, 201, 241, 20));
        pushButton_2 = new QPushButton(teacher);
        pushButton_2->setObjectName("pushButton_2");
        pushButton_2->setGeometry(QRect(29, 267, 91, 21));

        retranslateUi(teacher);

        QMetaObject::connectSlotsByName(teacher);
    } // setupUi

    void retranslateUi(QDialog *teacher)
    {
        teacher->setWindowTitle(QCoreApplication::translate("teacher", "Dialog", nullptr));
        lineEdit->setText(QCoreApplication::translate("teacher", "wasd\347\247\273\345\212\250\357\274\214j\346\224\273\345\207\273\357\274\214\346\273\241\350\241\200\346\227\266\347\251\272\346\240\274\351\227\252\351\201\277", nullptr));
        lineEdit_2->setText(QCoreApplication::translate("teacher", "\346\234\211\344\270\211\350\247\222\345\275\242\347\232\204\346\230\257\344\274\244\345\256\263\351\231\267\351\230\261", nullptr));
        lineEdit_3->setText(QCoreApplication::translate("teacher", "\346\224\273\345\207\273\345\274\200\345\205\263\345\217\257\344\273\245\346\211\223\345\274\200\345\274\200\345\205\263\351\227\250", nullptr));
        pushButton_2->setText(QCoreApplication::translate("teacher", "\345\233\236\345\210\260\345\210\235\345\247\213\351\241\265\351\235\242", nullptr));
    } // retranslateUi

};

namespace Ui {
    class teacher: public Ui_teacher {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_TEACHER_H
