/********************************************************************************
** Form generated from reading UI file 'HistogramDialog.ui'
**
** Created by: Qt User Interface Compiler version 6.11.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_HISTOGRAMDIALOG_H
#define UI_HISTOGRAMDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAbstractButton>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QVBoxLayout>
#include "histogramwidget.h"

QT_BEGIN_NAMESPACE

class Ui_HistogramDialog
{
public:
    QVBoxLayout *verticalLayout;
    QGridLayout *gridLayout;
    HistogramWidget *widget;
    HistogramWidget *widget_2;
    HistogramWidget *widget_3;
    HistogramWidget *widget_4;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *HistogramDialog)
    {
        if (HistogramDialog->objectName().isEmpty())
            HistogramDialog->setObjectName("HistogramDialog");
        HistogramDialog->resize(1350, 800);
        verticalLayout = new QVBoxLayout(HistogramDialog);
        verticalLayout->setObjectName("verticalLayout");
        gridLayout = new QGridLayout();
        gridLayout->setObjectName("gridLayout");
        widget = new HistogramWidget(HistogramDialog);
        widget->setObjectName("widget");
        QSizePolicy sizePolicy(QSizePolicy::Policy::Preferred, QSizePolicy::Policy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(widget->sizePolicy().hasHeightForWidth());
        widget->setSizePolicy(sizePolicy);

        gridLayout->addWidget(widget, 0, 0, 1, 1);

        widget_2 = new HistogramWidget(HistogramDialog);
        widget_2->setObjectName("widget_2");
        sizePolicy.setHeightForWidth(widget_2->sizePolicy().hasHeightForWidth());
        widget_2->setSizePolicy(sizePolicy);

        gridLayout->addWidget(widget_2, 0, 1, 1, 1);

        widget_3 = new HistogramWidget(HistogramDialog);
        widget_3->setObjectName("widget_3");
        sizePolicy.setHeightForWidth(widget_3->sizePolicy().hasHeightForWidth());
        widget_3->setSizePolicy(sizePolicy);

        gridLayout->addWidget(widget_3, 1, 0, 1, 1);

        widget_4 = new HistogramWidget(HistogramDialog);
        widget_4->setObjectName("widget_4");
        sizePolicy.setHeightForWidth(widget_4->sizePolicy().hasHeightForWidth());
        widget_4->setSizePolicy(sizePolicy);

        gridLayout->addWidget(widget_4, 1, 1, 1, 1);


        verticalLayout->addLayout(gridLayout);

        buttonBox = new QDialogButtonBox(HistogramDialog);
        buttonBox->setObjectName("buttonBox");
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        verticalLayout->addWidget(buttonBox);


        retranslateUi(HistogramDialog);
        QObject::connect(buttonBox, &QDialogButtonBox::accepted, HistogramDialog, qOverload<>(&QDialog::accept));
        QObject::connect(buttonBox, &QDialogButtonBox::rejected, HistogramDialog, qOverload<>(&QDialog::reject));

        QMetaObject::connectSlotsByName(HistogramDialog);
    } // setupUi

    void retranslateUi(QDialog *HistogramDialog)
    {
        HistogramDialog->setWindowTitle(QCoreApplication::translate("HistogramDialog", "Attribute Exploration", nullptr));
    } // retranslateUi

};

namespace Ui {
    class HistogramDialog: public Ui_HistogramDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_HISTOGRAMDIALOG_H
