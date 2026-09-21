/********************************************************************************
** Form generated from reading UI file 'timeselectionwidget.ui'
**
** Created by: Qt User Interface Compiler version 6.11.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_TIMESELECTIONWIDGET_H
#define UI_TIMESELECTIONWIDGET_H

#include <QtCore/QDate>
#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDateTimeEdit>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QWidget>
#include "timewidget.h"

QT_BEGIN_NAMESPACE

class Ui_TimeSelectionWidget
{
public:
    QGridLayout *gridLayout_2;
    QTabWidget *tabWidget;
    QWidget *regularTimeWidget;
    QHBoxLayout *horizontalLayout;
    QSpacerItem *horizontalSpacer_2;
    QGridLayout *gridLayout;
    QComboBox *comboBox_2;
    QPushButton *stepBackButton;
    QLabel *label_5;
    QLabel *label_6;
    QDateTimeEdit *endTime;
    QPushButton *stepForwardButton;
    QLabel *label_4;
    QDateTimeEdit *startTime;
    QSpacerItem *horizontalSpacer;
    TimeWidget *recurrentTimeWidget;
    QPushButton *exploreButton;
    QPushButton *queryButton;

    void setupUi(QWidget *TimeSelectionWidget)
    {
        if (TimeSelectionWidget->objectName().isEmpty())
            TimeSelectionWidget->setObjectName("TimeSelectionWidget");
        TimeSelectionWidget->resize(882, 140);
        TimeSelectionWidget->setMaximumSize(QSize(10000, 1000));
        gridLayout_2 = new QGridLayout(TimeSelectionWidget);
        gridLayout_2->setObjectName("gridLayout_2");
        tabWidget = new QTabWidget(TimeSelectionWidget);
        tabWidget->setObjectName("tabWidget");
        regularTimeWidget = new QWidget();
        regularTimeWidget->setObjectName("regularTimeWidget");
        horizontalLayout = new QHBoxLayout(regularTimeWidget);
        horizontalLayout->setSpacing(0);
        horizontalLayout->setObjectName("horizontalLayout");
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        horizontalSpacer_2 = new QSpacerItem(39, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout->addItem(horizontalSpacer_2);

        gridLayout = new QGridLayout();
        gridLayout->setSpacing(0);
        gridLayout->setObjectName("gridLayout");
        gridLayout->setSizeConstraint(QLayout::SetDefaultConstraint);
        comboBox_2 = new QComboBox(regularTimeWidget);
        comboBox_2->addItem(QString());
        comboBox_2->addItem(QString());
        comboBox_2->addItem(QString());
        comboBox_2->addItem(QString());
        comboBox_2->addItem(QString());
        comboBox_2->setObjectName("comboBox_2");
        comboBox_2->setMinimumSize(QSize(90, 0));

        gridLayout->addWidget(comboBox_2, 1, 3, 1, 1);

        stepBackButton = new QPushButton(regularTimeWidget);
        stepBackButton->setObjectName("stepBackButton");

        gridLayout->addWidget(stepBackButton, 1, 0, 1, 1);

        label_5 = new QLabel(regularTimeWidget);
        label_5->setObjectName("label_5");
        label_5->setAlignment(Qt::AlignCenter);

        gridLayout->addWidget(label_5, 0, 4, 1, 1);

        label_6 = new QLabel(regularTimeWidget);
        label_6->setObjectName("label_6");
        label_6->setAlignment(Qt::AlignCenter);

        gridLayout->addWidget(label_6, 0, 3, 1, 1);

        endTime = new QDateTimeEdit(regularTimeWidget);
        endTime->setObjectName("endTime");
        QSizePolicy sizePolicy(QSizePolicy::Policy::MinimumExpanding, QSizePolicy::Policy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(endTime->sizePolicy().hasHeightForWidth());
        endTime->setSizePolicy(sizePolicy);
        endTime->setMinimumSize(QSize(165, 0));
        endTime->setDateTime(QDateTime(QDate(2013, 1, 2), QTime(0, 0, 0)));
        endTime->setDate(QDate(2013, 1, 2));
        endTime->setTime(QTime(0, 0, 0));
        endTime->setCalendarPopup(true);

        gridLayout->addWidget(endTime, 1, 4, 1, 1);

        stepForwardButton = new QPushButton(regularTimeWidget);
        stepForwardButton->setObjectName("stepForwardButton");

        gridLayout->addWidget(stepForwardButton, 1, 5, 1, 1);

        label_4 = new QLabel(regularTimeWidget);
        label_4->setObjectName("label_4");
        label_4->setAlignment(Qt::AlignCenter);

        gridLayout->addWidget(label_4, 0, 1, 1, 2);

        startTime = new QDateTimeEdit(regularTimeWidget);
        startTime->setObjectName("startTime");
        sizePolicy.setHeightForWidth(startTime->sizePolicy().hasHeightForWidth());
        startTime->setSizePolicy(sizePolicy);
        startTime->setMinimumSize(QSize(165, 0));
        startTime->setDateTime(QDateTime(QDate(2013, 1, 1), QTime(0, 0, 0)));
        startTime->setDate(QDate(2013, 1, 1));
        startTime->setCalendarPopup(true);

        gridLayout->addWidget(startTime, 1, 1, 1, 1);


        horizontalLayout->addLayout(gridLayout);

        horizontalSpacer = new QSpacerItem(22, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        tabWidget->addTab(regularTimeWidget, QString());
        recurrentTimeWidget = new TimeWidget();
        recurrentTimeWidget->setObjectName("recurrentTimeWidget");
        tabWidget->addTab(recurrentTimeWidget, QString());

        gridLayout_2->addWidget(tabWidget, 0, 0, 3, 1);

        exploreButton = new QPushButton(TimeSelectionWidget);
        exploreButton->setObjectName("exploreButton");

        gridLayout_2->addWidget(exploreButton, 2, 1, 1, 1);

        queryButton = new QPushButton(TimeSelectionWidget);
        queryButton->setObjectName("queryButton");
        queryButton->setAutoDefault(false);
        queryButton->setFlat(false);

        gridLayout_2->addWidget(queryButton, 1, 1, 1, 1);


        retranslateUi(TimeSelectionWidget);
        QObject::connect(stepForwardButton, SIGNAL(clicked()), TimeSelectionWidget, SLOT(stepForward()));
        QObject::connect(stepBackButton, SIGNAL(clicked()), TimeSelectionWidget, SLOT(stepBack()));

        tabWidget->setCurrentIndex(0);
        comboBox_2->setCurrentIndex(2);
        queryButton->setDefault(false);


        QMetaObject::connectSlotsByName(TimeSelectionWidget);
    } // setupUi

    void retranslateUi(QWidget *TimeSelectionWidget)
    {
        TimeSelectionWidget->setWindowTitle(QCoreApplication::translate("TimeSelectionWidget", "Form", nullptr));
        comboBox_2->setItemText(0, QCoreApplication::translate("TimeSelectionWidget", "15 min", nullptr));
        comboBox_2->setItemText(1, QCoreApplication::translate("TimeSelectionWidget", "30 min", nullptr));
        comboBox_2->setItemText(2, QCoreApplication::translate("TimeSelectionWidget", "1 hour", nullptr));
        comboBox_2->setItemText(3, QCoreApplication::translate("TimeSelectionWidget", "1 day", nullptr));
        comboBox_2->setItemText(4, QCoreApplication::translate("TimeSelectionWidget", "1 week", nullptr));

        stepBackButton->setText(QString());
        label_5->setText(QCoreApplication::translate("TimeSelectionWidget", "End Time", nullptr));
        label_6->setText(QCoreApplication::translate("TimeSelectionWidget", "Step Size", nullptr));
        endTime->setDisplayFormat(QCoreApplication::translate("TimeSelectionWidget", "ddd MM/dd/yy hh:mm ", nullptr));
        stepForwardButton->setText(QString());
        label_4->setText(QCoreApplication::translate("TimeSelectionWidget", "Start Time", nullptr));
        startTime->setDisplayFormat(QCoreApplication::translate("TimeSelectionWidget", "ddd MM/dd/yy hh:mm ", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(regularTimeWidget), QCoreApplication::translate("TimeSelectionWidget", "Regular Selection", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(recurrentTimeWidget), QCoreApplication::translate("TimeSelectionWidget", "Recurrent Selection", nullptr));
        exploreButton->setText(QCoreApplication::translate("TimeSelectionWidget", "Explore...", nullptr));
        queryButton->setText(QCoreApplication::translate("TimeSelectionWidget", "Query", nullptr));
    } // retranslateUi

};

namespace Ui {
    class TimeSelectionWidget: public Ui_TimeSelectionWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_TIMESELECTIONWIDGET_H
