/********************************************************************************
** Form generated from reading UI file 'temporalseriesplotwidget.ui'
**
** Created by: Qt User Interface Compiler version 6.11.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_TEMPORALSERIESPLOTWIDGET_H
#define UI_TEMPORALSERIESPLOTWIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include "extendedplotwidget.h"

QT_BEGIN_NAMESPACE

class Ui_PlotWidget
{
public:
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout;
    QSpacerItem *horizontalSpacer_3;
    QGridLayout *gridLayout;
    QLabel *label_3;
    QComboBox *attributeComboBox;
    QSpacerItem *horizontalSpacer;
    QLabel *label;
    QSpinBox *numBinsSpin;
    QSpacerItem *horizontalSpacer_4;
    ExtendedPlotWidget *customPlot;

    void setupUi(QWidget *PlotWidget)
    {
        if (PlotWidget->objectName().isEmpty())
            PlotWidget->setObjectName("PlotWidget");
        PlotWidget->resize(808, 313);
        verticalLayout = new QVBoxLayout(PlotWidget);
        verticalLayout->setSpacing(0);
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        verticalLayout->setObjectName("verticalLayout");
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(0);
        horizontalLayout->setObjectName("horizontalLayout");
        horizontalSpacer_3 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout->addItem(horizontalSpacer_3);

        gridLayout = new QGridLayout();
        gridLayout->setObjectName("gridLayout");
        label_3 = new QLabel(PlotWidget);
        label_3->setObjectName("label_3");

        gridLayout->addWidget(label_3, 0, 0, 1, 1);

        attributeComboBox = new QComboBox(PlotWidget);
        attributeComboBox->addItem(QString());
        attributeComboBox->addItem(QString());
        attributeComboBox->addItem(QString());
        attributeComboBox->addItem(QString());
        attributeComboBox->addItem(QString());
        attributeComboBox->addItem(QString());
        attributeComboBox->addItem(QString());
        attributeComboBox->addItem(QString());
        attributeComboBox->addItem(QString());
        attributeComboBox->addItem(QString());
        attributeComboBox->addItem(QString());
        attributeComboBox->addItem(QString());
        attributeComboBox->addItem(QString());
        attributeComboBox->addItem(QString());
        attributeComboBox->setObjectName("attributeComboBox");

        gridLayout->addWidget(attributeComboBox, 0, 1, 1, 1);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        gridLayout->addItem(horizontalSpacer, 0, 2, 1, 1);

        label = new QLabel(PlotWidget);
        label->setObjectName("label");

        gridLayout->addWidget(label, 0, 3, 1, 1);

        numBinsSpin = new QSpinBox(PlotWidget);
        numBinsSpin->setObjectName("numBinsSpin");
        numBinsSpin->setMinimum(1);
        numBinsSpin->setMaximum(10000);
        numBinsSpin->setValue(10);

        gridLayout->addWidget(numBinsSpin, 0, 4, 1, 1);


        horizontalLayout->addLayout(gridLayout);

        horizontalSpacer_4 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout->addItem(horizontalSpacer_4);


        verticalLayout->addLayout(horizontalLayout);

        customPlot = new ExtendedPlotWidget(PlotWidget);
        customPlot->setObjectName("customPlot");

        verticalLayout->addWidget(customPlot);

        verticalLayout->setStretch(1, 2);

        retranslateUi(PlotWidget);

        QMetaObject::connectSlotsByName(PlotWidget);
    } // setupUi

    void retranslateUi(QWidget *PlotWidget)
    {
        PlotWidget->setWindowTitle(QCoreApplication::translate("PlotWidget", "Form", nullptr));
        label_3->setText(QCoreApplication::translate("PlotWidget", "Attribute: ", nullptr));
        attributeComboBox->setItemText(0, QCoreApplication::translate("PlotWidget", "Num Trips", nullptr));
        attributeComboBox->setItemText(1, QCoreApplication::translate("PlotWidget", "Fare Amount", nullptr));
        attributeComboBox->setItemText(2, QCoreApplication::translate("PlotWidget", "Tip", nullptr));
        attributeComboBox->setItemText(3, QCoreApplication::translate("PlotWidget", "Total Revenue", nullptr));
        attributeComboBox->setItemText(4, QCoreApplication::translate("PlotWidget", "Fare per Trip", nullptr));
        attributeComboBox->setItemText(5, QCoreApplication::translate("PlotWidget", "Fare per Mile", nullptr));
        attributeComboBox->setItemText(6, QCoreApplication::translate("PlotWidget", "Tips per Trip", nullptr));
        attributeComboBox->setItemText(7, QCoreApplication::translate("PlotWidget", "Tips per Mile", nullptr));
        attributeComboBox->setItemText(8, QCoreApplication::translate("PlotWidget", "Tolls Amount", nullptr));
        attributeComboBox->setItemText(9, QCoreApplication::translate("PlotWidget", "Tolls Per Trip", nullptr));
        attributeComboBox->setItemText(10, QCoreApplication::translate("PlotWidget", "Active Cabs", nullptr));
        attributeComboBox->setItemText(11, QCoreApplication::translate("PlotWidget", "Distance", nullptr));
        attributeComboBox->setItemText(12, QCoreApplication::translate("PlotWidget", "Duration", nullptr));
        attributeComboBox->setItemText(13, QCoreApplication::translate("PlotWidget", "Average Speed", nullptr));

        label->setText(QCoreApplication::translate("PlotWidget", "Num Bins:", nullptr));
    } // retranslateUi

};

namespace Ui {
    class PlotWidget: public Ui_PlotWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_TEMPORALSERIESPLOTWIDGET_H
