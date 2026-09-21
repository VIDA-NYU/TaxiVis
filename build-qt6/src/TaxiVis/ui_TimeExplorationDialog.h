/********************************************************************************
** Form generated from reading UI file 'TimeExplorationDialog.ui'
**
** Created by: Qt User Interface Compiler version 6.11.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_TIMEEXPLORATIONDIALOG_H
#define UI_TIMEEXPLORATIONDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAbstractButton>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QSplitter>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include "histogramwidget.h"
#include "scatterplotwidget.h"
#include "temporalseriesplotwidget.h"

QT_BEGIN_NAMESPACE

class Ui_TimeExplorationDialog
{
public:
    QVBoxLayout *verticalLayout;
    QSplitter *splitter;
    QWidget *widget;
    QHBoxLayout *mainLayout;
    QTabWidget *tabWidget;
    TemporalSeriesPlotWidget *timeSeriesWidget;
    HistogramWidget *histogramWidget;
    ScatterPlotWidget *scatterPlotWidget;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *TimeExplorationDialog)
    {
        if (TimeExplorationDialog->objectName().isEmpty())
            TimeExplorationDialog->setObjectName("TimeExplorationDialog");
        TimeExplorationDialog->resize(1280, 850);
        verticalLayout = new QVBoxLayout(TimeExplorationDialog);
        verticalLayout->setSpacing(0);
        verticalLayout->setObjectName("verticalLayout");
        splitter = new QSplitter(TimeExplorationDialog);
        splitter->setObjectName("splitter");
        splitter->setOrientation(Qt::Vertical);
        widget = new QWidget(splitter);
        widget->setObjectName("widget");
        mainLayout = new QHBoxLayout(widget);
        mainLayout->setObjectName("mainLayout");
        splitter->addWidget(widget);
        tabWidget = new QTabWidget(splitter);
        tabWidget->setObjectName("tabWidget");
        tabWidget->setMinimumSize(QSize(0, 250));
        timeSeriesWidget = new TemporalSeriesPlotWidget();
        timeSeriesWidget->setObjectName("timeSeriesWidget");
        tabWidget->addTab(timeSeriesWidget, QString());
        histogramWidget = new HistogramWidget();
        histogramWidget->setObjectName("histogramWidget");
        tabWidget->addTab(histogramWidget, QString());
        scatterPlotWidget = new ScatterPlotWidget();
        scatterPlotWidget->setObjectName("scatterPlotWidget");
        tabWidget->addTab(scatterPlotWidget, QString());
        splitter->addWidget(tabWidget);

        verticalLayout->addWidget(splitter);

        buttonBox = new QDialogButtonBox(TimeExplorationDialog);
        buttonBox->setObjectName("buttonBox");
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        verticalLayout->addWidget(buttonBox);


        retranslateUi(TimeExplorationDialog);
        QObject::connect(buttonBox, &QDialogButtonBox::accepted, TimeExplorationDialog, qOverload<>(&QDialog::accept));
        QObject::connect(buttonBox, &QDialogButtonBox::rejected, TimeExplorationDialog, qOverload<>(&QDialog::reject));

        tabWidget->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(TimeExplorationDialog);
    } // setupUi

    void retranslateUi(QDialog *TimeExplorationDialog)
    {
        TimeExplorationDialog->setWindowTitle(QCoreApplication::translate("TimeExplorationDialog", "Parameter Exploration In Time", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(timeSeriesWidget), QCoreApplication::translate("TimeExplorationDialog", "Time Series", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(histogramWidget), QCoreApplication::translate("TimeExplorationDialog", "Histogram", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(scatterPlotWidget), QCoreApplication::translate("TimeExplorationDialog", "ScatterPlot", nullptr));
    } // retranslateUi

};

namespace Ui {
    class TimeExplorationDialog: public Ui_TimeExplorationDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_TIMEEXPLORATIONDIALOG_H
