/********************************************************************************
** Form generated from reading UI file 'steakgameengine.ui'
**
** Created by: Qt User Interface Compiler version 6.9.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_STEAKGAMEENGINE_H
#define UI_STEAKGAMEENGINE_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_SteakGameEngine
{
public:
    QWidget *centralwidget;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *SteakGameEngine)
    {
        if (SteakGameEngine->objectName().isEmpty())
            SteakGameEngine->setObjectName("SteakGameEngine");
        SteakGameEngine->resize(800, 600);
        centralwidget = new QWidget(SteakGameEngine);
        centralwidget->setObjectName("centralwidget");
        SteakGameEngine->setCentralWidget(centralwidget);
        menubar = new QMenuBar(SteakGameEngine);
        menubar->setObjectName("menubar");
        menubar->setGeometry(QRect(0, 0, 800, 21));
        SteakGameEngine->setMenuBar(menubar);
        statusbar = new QStatusBar(SteakGameEngine);
        statusbar->setObjectName("statusbar");
        SteakGameEngine->setStatusBar(statusbar);

        retranslateUi(SteakGameEngine);

        QMetaObject::connectSlotsByName(SteakGameEngine);
    } // setupUi

    void retranslateUi(QMainWindow *SteakGameEngine)
    {
        SteakGameEngine->setWindowTitle(QCoreApplication::translate("SteakGameEngine", "SteakGameEngine", nullptr));
    } // retranslateUi

};

namespace Ui {
    class SteakGameEngine: public Ui_SteakGameEngine {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_STEAKGAMEENGINE_H
