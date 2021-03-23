/********************************************************************************
** Form generated from reading UI file 'conecta4.ui'
**
** Created by: Qt User Interface Compiler version 5.12.8
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CONECTA4_H
#define UI_CONECTA4_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QGraphicsView>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Conecta4
{
public:
    QWidget *centralwidget;
    QWidget *verticalLayoutWidget;
    QVBoxLayout *mainLayout;
    QHBoxLayout *horizontalLayout;
    QPushButton *startBt;
    QComboBox *gameTypeCombo;
    QGraphicsView *graphicsView;
    QLabel *infoLabel;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *Conecta4)
    {
        if (Conecta4->objectName().isEmpty())
            Conecta4->setObjectName(QString::fromUtf8("Conecta4"));
        Conecta4->resize(800, 600);
        centralwidget = new QWidget(Conecta4);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        verticalLayoutWidget = new QWidget(centralwidget);
        verticalLayoutWidget->setObjectName(QString::fromUtf8("verticalLayoutWidget"));
        verticalLayoutWidget->setGeometry(QRect(20, 10, 731, 521));
        mainLayout = new QVBoxLayout(verticalLayoutWidget);
        mainLayout->setObjectName(QString::fromUtf8("mainLayout"));
        mainLayout->setSizeConstraint(QLayout::SetMaximumSize);
        mainLayout->setContentsMargins(0, 0, 0, 0);
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        startBt = new QPushButton(verticalLayoutWidget);
        startBt->setObjectName(QString::fromUtf8("startBt"));

        horizontalLayout->addWidget(startBt);

        gameTypeCombo = new QComboBox(verticalLayoutWidget);
        gameTypeCombo->setObjectName(QString::fromUtf8("gameTypeCombo"));

        horizontalLayout->addWidget(gameTypeCombo);


        mainLayout->addLayout(horizontalLayout);

        graphicsView = new QGraphicsView(verticalLayoutWidget);
        graphicsView->setObjectName(QString::fromUtf8("graphicsView"));

        mainLayout->addWidget(graphicsView);

        infoLabel = new QLabel(verticalLayoutWidget);
        infoLabel->setObjectName(QString::fromUtf8("infoLabel"));

        mainLayout->addWidget(infoLabel);

        Conecta4->setCentralWidget(centralwidget);
        menubar = new QMenuBar(Conecta4);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 800, 34));
        Conecta4->setMenuBar(menubar);
        statusbar = new QStatusBar(Conecta4);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        Conecta4->setStatusBar(statusbar);

        retranslateUi(Conecta4);

        QMetaObject::connectSlotsByName(Conecta4);
    } // setupUi

    void retranslateUi(QMainWindow *Conecta4)
    {
        Conecta4->setWindowTitle(QApplication::translate("Conecta4", "Conecta4 - Pr\303\241ctica 1 - SI 2021", nullptr));
        startBt->setText(QApplication::translate("Conecta4", "PushButton", nullptr));
        infoLabel->setText(QApplication::translate("Conecta4", "TextLabel", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Conecta4: public Ui_Conecta4 {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CONECTA4_H
