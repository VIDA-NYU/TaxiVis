/********************************************************************************
** Form generated from reading UI file 'histogramwidget.ui'
**
** Created by: Qt User Interface Compiler version 6.11.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_HISTOGRAMWIDGET_H
#define UI_HISTOGRAMWIDGET_H

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
#include "extendedhistogram.h"

QT_BEGIN_NAMESPACE

class Ui_HistogramWidget
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
    QSpinBox *numBinsSpinBox;
    QSpacerItem *horizontalSpacer_4;
    ExtendedHistogram *histogram;

    void setupUi(QWidget *HistogramWidget)
    {
        if (HistogramWidget->objectName().isEmpty())
            HistogramWidget->setObjectName("HistogramWidget");
        HistogramWidget->resize(808, 313);
        verticalLayout = new QVBoxLayout(HistogramWidget);
        verticalLayout->setObjectName("verticalLayout");
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(0);
        horizontalLayout->setObjectName("horizontalLayout");
        horizontalSpacer_3 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout->addItem(horizontalSpacer_3);

        gridLayout = new QGridLayout();
        gridLayout->setObjectName("gridLayout");
        label_3 = new QLabel(HistogramWidget);
        label_3->setObjectName("label_3");

        gridLayout->addWidget(label_3, 0, 0, 1, 1);

        attributeComboBox = new QComboBox(HistogramWidget);
        attributeComboBox->addItem(QString());
        attributeComboBox->addItem(QString());
        attributeComboBox->addItem(QString());
        attributeComboBox->addItem(QString());
        attributeComboBox->setObjectName("attributeComboBox");

        gridLayout->addWidget(attributeComboBox, 0, 1, 1, 1);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        gridLayout->addItem(horizontalSpacer, 0, 2, 1, 1);

        label = new QLabel(HistogramWidget);
        label->setObjectName("label");

        gridLayout->addWidget(label, 0, 3, 1, 1);

        numBinsSpinBox = new QSpinBox(HistogramWidget);
        numBinsSpinBox->setObjectName("numBinsSpinBox");
        numBinsSpinBox->setMinimum(1);
        numBinsSpinBox->setMaximum(10000);
        numBinsSpinBox->setValue(10);

        gridLayout->addWidget(numBinsSpinBox, 0, 4, 1, 1);


        horizontalLayout->addLayout(gridLayout);

        horizontalSpacer_4 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout->addItem(horizontalSpacer_4);


        verticalLayout->addLayout(horizontalLayout);

        histogram = new ExtendedHistogram(HistogramWidget);
        histogram->setObjectName("histogram");

        verticalLayout->addWidget(histogram);

        verticalLayout->setStretch(1, 2);

        retranslateUi(HistogramWidget);
        QObject::connect(attributeComboBox, SIGNAL(currentTextChanged(QString)), HistogramWidget, SLOT(changeHistAttribute(QString)));
        QObject::connect(numBinsSpinBox, SIGNAL(valueChanged(int)), HistogramWidget, SLOT(setNumberOfBins(int)));

        QMetaObject::connectSlotsByName(HistogramWidget);
    } // setupUi

    void retranslateUi(QWidget *HistogramWidget)
    {
        HistogramWidget->setWindowTitle(QCoreApplication::translate("HistogramWidget", "Form", nullptr));
        label_3->setText(QCoreApplication::translate("HistogramWidget", "Attribute: ", nullptr));
        attributeComboBox->setItemText(0, QCoreApplication::translate("HistogramWidget", "Fare Amount", nullptr));
        attributeComboBox->setItemText(1, QCoreApplication::translate("HistogramWidget", "Tip", nullptr));
        attributeComboBox->setItemText(2, QCoreApplication::translate("HistogramWidget", "Distance", nullptr));
        attributeComboBox->setItemText(3, QCoreApplication::translate("HistogramWidget", "Duration", nullptr));

        label->setText(QCoreApplication::translate("HistogramWidget", "Num Bins", nullptr));
    } // retranslateUi

};

namespace Ui {
    class HistogramWidget: public Ui_HistogramWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_HISTOGRAMWIDGET_H
