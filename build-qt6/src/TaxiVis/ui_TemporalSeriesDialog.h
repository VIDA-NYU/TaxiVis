/********************************************************************************
** Form generated from reading UI file 'TemporalSeriesDialog.ui'
**
** Created by: Qt User Interface Compiler version 6.11.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_TEMPORALSERIESDIALOG_H
#define UI_TEMPORALSERIESDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAbstractButton>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QVBoxLayout>
#include "temporalseriesplotwidget.h"

QT_BEGIN_NAMESPACE

class Ui_TemporalSeriesDialog
{
public:
    QVBoxLayout *verticalLayout;
    QGridLayout *gridLayout;
    TemporalSeriesPlotWidget *widget_2;
    TemporalSeriesPlotWidget *widget_8;
    TemporalSeriesPlotWidget *widget_9;
    TemporalSeriesPlotWidget *widget_7;
    TemporalSeriesPlotWidget *widget_10;
    TemporalSeriesPlotWidget *widget_4;
    TemporalSeriesPlotWidget *widget_5;
    TemporalSeriesPlotWidget *widget_14;
    TemporalSeriesPlotWidget *widget_1;
    TemporalSeriesPlotWidget *widget_6;
    TemporalSeriesPlotWidget *widget_11;
    TemporalSeriesPlotWidget *widget_13;
    TemporalSeriesPlotWidget *widget_3;
    TemporalSeriesPlotWidget *widget_12;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *TemporalSeriesDialog)
    {
        if (TemporalSeriesDialog->objectName().isEmpty())
            TemporalSeriesDialog->setObjectName("TemporalSeriesDialog");
        TemporalSeriesDialog->resize(1440, 800);
        verticalLayout = new QVBoxLayout(TemporalSeriesDialog);
        verticalLayout->setObjectName("verticalLayout");
        gridLayout = new QGridLayout();
        gridLayout->setObjectName("gridLayout");
        gridLayout->setVerticalSpacing(0);
        widget_2 = new TemporalSeriesPlotWidget(TemporalSeriesDialog);
        widget_2->setObjectName("widget_2");

        gridLayout->addWidget(widget_2, 0, 1, 1, 1);

        widget_8 = new TemporalSeriesPlotWidget(TemporalSeriesDialog);
        widget_8->setObjectName("widget_8");

        gridLayout->addWidget(widget_8, 2, 1, 1, 1);

        widget_9 = new TemporalSeriesPlotWidget(TemporalSeriesDialog);
        widget_9->setObjectName("widget_9");

        gridLayout->addWidget(widget_9, 2, 2, 1, 1);

        widget_7 = new TemporalSeriesPlotWidget(TemporalSeriesDialog);
        widget_7->setObjectName("widget_7");

        gridLayout->addWidget(widget_7, 2, 0, 1, 1);

        widget_10 = new TemporalSeriesPlotWidget(TemporalSeriesDialog);
        widget_10->setObjectName("widget_10");

        gridLayout->addWidget(widget_10, 3, 0, 1, 1);

        widget_4 = new TemporalSeriesPlotWidget(TemporalSeriesDialog);
        widget_4->setObjectName("widget_4");

        gridLayout->addWidget(widget_4, 1, 0, 1, 1);

        widget_5 = new TemporalSeriesPlotWidget(TemporalSeriesDialog);
        widget_5->setObjectName("widget_5");

        gridLayout->addWidget(widget_5, 1, 1, 1, 1);

        widget_14 = new TemporalSeriesPlotWidget(TemporalSeriesDialog);
        widget_14->setObjectName("widget_14");

        gridLayout->addWidget(widget_14, 4, 1, 1, 1);

        widget_1 = new TemporalSeriesPlotWidget(TemporalSeriesDialog);
        widget_1->setObjectName("widget_1");

        gridLayout->addWidget(widget_1, 0, 0, 1, 1);

        widget_6 = new TemporalSeriesPlotWidget(TemporalSeriesDialog);
        widget_6->setObjectName("widget_6");

        gridLayout->addWidget(widget_6, 1, 2, 1, 1);

        widget_11 = new TemporalSeriesPlotWidget(TemporalSeriesDialog);
        widget_11->setObjectName("widget_11");

        gridLayout->addWidget(widget_11, 3, 1, 1, 1);

        widget_13 = new TemporalSeriesPlotWidget(TemporalSeriesDialog);
        widget_13->setObjectName("widget_13");

        gridLayout->addWidget(widget_13, 4, 0, 1, 1);

        widget_3 = new TemporalSeriesPlotWidget(TemporalSeriesDialog);
        widget_3->setObjectName("widget_3");

        gridLayout->addWidget(widget_3, 0, 2, 1, 1);

        widget_12 = new TemporalSeriesPlotWidget(TemporalSeriesDialog);
        widget_12->setObjectName("widget_12");

        gridLayout->addWidget(widget_12, 3, 2, 1, 1);


        verticalLayout->addLayout(gridLayout);

        buttonBox = new QDialogButtonBox(TemporalSeriesDialog);
        buttonBox->setObjectName("buttonBox");
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        verticalLayout->addWidget(buttonBox);


        retranslateUi(TemporalSeriesDialog);
        QObject::connect(buttonBox, &QDialogButtonBox::accepted, TemporalSeriesDialog, qOverload<>(&QDialog::accept));
        QObject::connect(buttonBox, &QDialogButtonBox::rejected, TemporalSeriesDialog, qOverload<>(&QDialog::reject));

        QMetaObject::connectSlotsByName(TemporalSeriesDialog);
    } // setupUi

    void retranslateUi(QDialog *TemporalSeriesDialog)
    {
        TemporalSeriesDialog->setWindowTitle(QCoreApplication::translate("TemporalSeriesDialog", "Attribute Exploration", nullptr));
    } // retranslateUi

};

namespace Ui {
    class TemporalSeriesDialog: public Ui_TemporalSeriesDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_TEMPORALSERIESDIALOG_H
