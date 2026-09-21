/********************************************************************************
** Form generated from reading UI file 'scatterplotwidget.ui'
**
** Created by: Qt User Interface Compiler version 6.11.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SCATTERPLOTWIDGET_H
#define UI_SCATTERPLOTWIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include "qcustomplot.h"

QT_BEGIN_NAMESPACE

class Ui_ScatterPlotWidget
{
public:
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout;
    QSpacerItem *horizontalSpacer_3;
    QLabel *label;
    QComboBox *comboBox;
    QSpacerItem *horizontalSpacer;
    QLabel *label_2;
    QComboBox *comboBox_2;
    QSpacerItem *horizontalSpacer_2;
    QCustomPlot *customPlot;

    void setupUi(QWidget *ScatterPlotWidget)
    {
        if (ScatterPlotWidget->objectName().isEmpty())
            ScatterPlotWidget->setObjectName("ScatterPlotWidget");
        ScatterPlotWidget->resize(808, 313);
        verticalLayout = new QVBoxLayout(ScatterPlotWidget);
        verticalLayout->setObjectName("verticalLayout");
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName("horizontalLayout");
        horizontalSpacer_3 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout->addItem(horizontalSpacer_3);

        label = new QLabel(ScatterPlotWidget);
        label->setObjectName("label");

        horizontalLayout->addWidget(label);

        comboBox = new QComboBox(ScatterPlotWidget);
        comboBox->addItem(QString());
        comboBox->addItem(QString());
        comboBox->addItem(QString());
        comboBox->addItem(QString());
        comboBox->addItem(QString());
        comboBox->addItem(QString());
        comboBox->addItem(QString());
        comboBox->setObjectName("comboBox");
        comboBox->setMinimumSize(QSize(130, 0));

        horizontalLayout->addWidget(comboBox);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        label_2 = new QLabel(ScatterPlotWidget);
        label_2->setObjectName("label_2");

        horizontalLayout->addWidget(label_2);

        comboBox_2 = new QComboBox(ScatterPlotWidget);
        comboBox_2->addItem(QString());
        comboBox_2->addItem(QString());
        comboBox_2->addItem(QString());
        comboBox_2->addItem(QString());
        comboBox_2->addItem(QString());
        comboBox_2->addItem(QString());
        comboBox_2->addItem(QString());
        comboBox_2->setObjectName("comboBox_2");
        comboBox_2->setMinimumSize(QSize(130, 0));

        horizontalLayout->addWidget(comboBox_2);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout->addItem(horizontalSpacer_2);


        verticalLayout->addLayout(horizontalLayout);

        customPlot = new QCustomPlot(ScatterPlotWidget);
        customPlot->setObjectName("customPlot");

        verticalLayout->addWidget(customPlot);

        verticalLayout->setStretch(1, 2);

        retranslateUi(ScatterPlotWidget);
        QObject::connect(comboBox, SIGNAL(currentTextChanged(QString)), ScatterPlotWidget, SLOT(setAttribute1(QString)));
        QObject::connect(comboBox_2, SIGNAL(currentTextChanged(QString)), ScatterPlotWidget, SLOT(setAttribute2(QString)));

        comboBox_2->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(ScatterPlotWidget);
    } // setupUi

    void retranslateUi(QWidget *ScatterPlotWidget)
    {
        ScatterPlotWidget->setWindowTitle(QCoreApplication::translate("ScatterPlotWidget", "Form", nullptr));
        label->setText(QCoreApplication::translate("ScatterPlotWidget", "Attribute 1:", nullptr));
        comboBox->setItemText(0, QCoreApplication::translate("ScatterPlotWidget", "Fare Amount", nullptr));
        comboBox->setItemText(1, QCoreApplication::translate("ScatterPlotWidget", "Tip Amount", nullptr));
        comboBox->setItemText(2, QCoreApplication::translate("ScatterPlotWidget", "Distance", nullptr));
        comboBox->setItemText(3, QCoreApplication::translate("ScatterPlotWidget", "Duration", nullptr));
        comboBox->setItemText(4, QCoreApplication::translate("ScatterPlotWidget", "Tolls Amount", nullptr));
        comboBox->setItemText(5, QCoreApplication::translate("ScatterPlotWidget", "Average Speed", nullptr));
        comboBox->setItemText(6, QCoreApplication::translate("ScatterPlotWidget", "Hour of Day", nullptr));

        label_2->setText(QCoreApplication::translate("ScatterPlotWidget", "Attribute 2:", nullptr));
        comboBox_2->setItemText(0, QCoreApplication::translate("ScatterPlotWidget", "Fare Amount", nullptr));
        comboBox_2->setItemText(1, QCoreApplication::translate("ScatterPlotWidget", "Tip Amount", nullptr));
        comboBox_2->setItemText(2, QCoreApplication::translate("ScatterPlotWidget", "Distance", nullptr));
        comboBox_2->setItemText(3, QCoreApplication::translate("ScatterPlotWidget", "Duration", nullptr));
        comboBox_2->setItemText(4, QCoreApplication::translate("ScatterPlotWidget", "Tolls Amount", nullptr));
        comboBox_2->setItemText(5, QCoreApplication::translate("ScatterPlotWidget", "Average Speed", nullptr));
        comboBox_2->setItemText(6, QCoreApplication::translate("ScatterPlotWidget", "Hour of Day", nullptr));

    } // retranslateUi

};

namespace Ui {
    class ScatterPlotWidget: public Ui_ScatterPlotWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SCATTERPLOTWIDGET_H
