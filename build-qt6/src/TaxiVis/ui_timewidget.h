/********************************************************************************
** Form generated from reading UI file 'timewidget.ui'
**
** Created by: Qt User Interface Compiler version 6.11.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_TIMEWIDGET_H
#define UI_TIMEWIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include "timewidget.h"

QT_BEGIN_NAMESPACE

class Ui_TimeWidget
{
public:
    QVBoxLayout *verticalLayout;
    CellsWidget *widgetYears;
    CellsWidget *widgetMonths;
    CellsWidget *widgetDays;
    CellsWidget *widgetHours;

    void setupUi(QWidget *TimeWidget)
    {
        if (TimeWidget->objectName().isEmpty())
            TimeWidget->setObjectName("TimeWidget");
        TimeWidget->resize(602, 60);
        TimeWidget->setMinimumSize(QSize(0, 60));
        TimeWidget->setMaximumSize(QSize(16777215, 60));
        verticalLayout = new QVBoxLayout(TimeWidget);
        verticalLayout->setSpacing(0);
        verticalLayout->setObjectName("verticalLayout");
        verticalLayout->setContentsMargins(12, 0, 12, 0);
        widgetYears = new CellsWidget(TimeWidget);
        widgetYears->setObjectName("widgetYears");

        verticalLayout->addWidget(widgetYears);

        widgetMonths = new CellsWidget(TimeWidget);
        widgetMonths->setObjectName("widgetMonths");

        verticalLayout->addWidget(widgetMonths);

        widgetDays = new CellsWidget(TimeWidget);
        widgetDays->setObjectName("widgetDays");

        verticalLayout->addWidget(widgetDays);

        widgetHours = new CellsWidget(TimeWidget);
        widgetHours->setObjectName("widgetHours");

        verticalLayout->addWidget(widgetHours);


        retranslateUi(TimeWidget);

        QMetaObject::connectSlotsByName(TimeWidget);
    } // setupUi

    void retranslateUi(QWidget *TimeWidget)
    {
        TimeWidget->setWindowTitle(QCoreApplication::translate("TimeWidget", "Form", nullptr));
    } // retranslateUi

};

namespace Ui {
    class TimeWidget: public Ui_TimeWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_TIMEWIDGET_H
