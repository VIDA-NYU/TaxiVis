/********************************************************************************
** Form generated from reading UI file 'viewwidget.ui'
**
** Created by: Qt User Interface Compiler version 6.11.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_VIEWWIDGET_H
#define UI_VIEWWIDGET_H

#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSplitter>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include "geographicalviewwidget.h"
#include "histogramwidget.h"
#include "scatterplotwidget.h"
#include "temporalseriesplotwidget.h"
#include "timeselectionwidget.h"

QT_BEGIN_NAMESPACE

class Ui_ViewWidget
{
public:
    QVBoxLayout *verticalLayout_2;
    QHBoxLayout *horizontalLayout_2;
    TimeSelectionWidget *timeSelectionWidget;
    QToolButton *syncButton;
    QSplitter *splitter;
    QWidget *layoutWidget;
    QHBoxLayout *horizontalLayout;
    GeographicalViewWidget *geographicalView;
    QWidget *widget;
    QVBoxLayout *verticalLayout;
    QToolButton *pickupModeButton;
    QToolButton *dropoffModeButton;
    QToolButton *pickupAndDropoffButton;
    QToolButton *linkModeButton;
    QToolButton *mergeButton;
    QToolButton *unmergeButton;
    QSpacerItem *verticalSpacer_2;
    QToolButton *showMapButton;
    QToolButton *showAnimationButton;
    QToolButton *exportButton;
    QToolButton *exploreButton;
    QSpacerItem *verticalSpacer;
    QTabWidget *tabWidget;
    TemporalSeriesPlotWidget *timeSeriesWidget;
    HistogramWidget *histogramWidget;
    ScatterPlotWidget *scatterPlotWidget;

    void setupUi(QWidget *ViewWidget)
    {
        if (ViewWidget->objectName().isEmpty())
            ViewWidget->setObjectName("ViewWidget");
        ViewWidget->setWindowModality(Qt::NonModal);
        ViewWidget->resize(752, 590);
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/Resources/icons/map.png"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
        ViewWidget->setWindowIcon(icon);
        verticalLayout_2 = new QVBoxLayout(ViewWidget);
        verticalLayout_2->setObjectName("verticalLayout_2");
        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName("horizontalLayout_2");
        timeSelectionWidget = new TimeSelectionWidget(ViewWidget);
        timeSelectionWidget->setObjectName("timeSelectionWidget");

        horizontalLayout_2->addWidget(timeSelectionWidget);

        syncButton = new QToolButton(ViewWidget);
        syncButton->setObjectName("syncButton");
        QIcon icon1;
        icon1.addFile(QString::fromUtf8(":/Resources/icons/sync.gray.png"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
        icon1.addFile(QString::fromUtf8(":/Resources/icons/sync.color.png"), QSize(), QIcon::Mode::Normal, QIcon::State::On);
        syncButton->setIcon(icon1);
        syncButton->setCheckable(true);

        horizontalLayout_2->addWidget(syncButton);


        verticalLayout_2->addLayout(horizontalLayout_2);

        splitter = new QSplitter(ViewWidget);
        splitter->setObjectName("splitter");
        splitter->setOrientation(Qt::Vertical);
        layoutWidget = new QWidget(splitter);
        layoutWidget->setObjectName("layoutWidget");
        horizontalLayout = new QHBoxLayout(layoutWidget);
        horizontalLayout->setObjectName("horizontalLayout");
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        geographicalView = new GeographicalViewWidget(layoutWidget);
        geographicalView->setObjectName("geographicalView");

        horizontalLayout->addWidget(geographicalView);

        widget = new QWidget(layoutWidget);
        widget->setObjectName("widget");
        widget->setMinimumSize(QSize(36, 0));
        widget->setMaximumSize(QSize(36, 16777215));
        verticalLayout = new QVBoxLayout(widget);
        verticalLayout->setObjectName("verticalLayout");
        verticalLayout->setContentsMargins(3, 3, 3, 3);
        pickupModeButton = new QToolButton(widget);
        pickupModeButton->setObjectName("pickupModeButton");
        pickupModeButton->setMinimumSize(QSize(30, 30));
        pickupModeButton->setMaximumSize(QSize(30, 30));
        QIcon icon2;
        icon2.addFile(QString::fromUtf8(":/Resources/icons/pickup.png"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
        icon2.addFile(QString::fromUtf8(":/Resources/icons/pickup.png"), QSize(), QIcon::Mode::Normal, QIcon::State::On);
        icon2.addFile(QString::fromUtf8(":/Resources/icons/pickup.png"), QSize(), QIcon::Mode::Selected, QIcon::State::Off);
        pickupModeButton->setIcon(icon2);
        pickupModeButton->setIconSize(QSize(22, 22));
        pickupModeButton->setCheckable(true);
        pickupModeButton->setChecked(true);

        verticalLayout->addWidget(pickupModeButton);

        dropoffModeButton = new QToolButton(widget);
        dropoffModeButton->setObjectName("dropoffModeButton");
        dropoffModeButton->setMinimumSize(QSize(30, 30));
        dropoffModeButton->setMaximumSize(QSize(30, 30));
        QIcon icon3;
        icon3.addFile(QString::fromUtf8(":/Resources/icons/dropoff.png"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
        dropoffModeButton->setIcon(icon3);
        dropoffModeButton->setIconSize(QSize(22, 22));
        dropoffModeButton->setCheckable(true);

        verticalLayout->addWidget(dropoffModeButton);

        pickupAndDropoffButton = new QToolButton(widget);
        pickupAndDropoffButton->setObjectName("pickupAndDropoffButton");
        pickupAndDropoffButton->setMinimumSize(QSize(30, 30));
        pickupAndDropoffButton->setMaximumSize(QSize(30, 30));
        QIcon icon4;
        icon4.addFile(QString::fromUtf8(":/Resources/icons/pickup_dropoff.png"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
        pickupAndDropoffButton->setIcon(icon4);
        pickupAndDropoffButton->setIconSize(QSize(22, 22));
        pickupAndDropoffButton->setCheckable(true);

        verticalLayout->addWidget(pickupAndDropoffButton);

        linkModeButton = new QToolButton(widget);
        linkModeButton->setObjectName("linkModeButton");
        linkModeButton->setMinimumSize(QSize(30, 30));
        linkModeButton->setMaximumSize(QSize(30, 30));
        QIcon icon5;
        icon5.addFile(QString::fromUtf8(":/Resources/icons/link.png"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
        linkModeButton->setIcon(icon5);
        linkModeButton->setIconSize(QSize(22, 22));
        linkModeButton->setCheckable(true);
        linkModeButton->setAutoExclusive(false);

        verticalLayout->addWidget(linkModeButton);

        mergeButton = new QToolButton(widget);
        mergeButton->setObjectName("mergeButton");
        mergeButton->setMinimumSize(QSize(30, 30));
        mergeButton->setMaximumSize(QSize(30, 30));
        QIcon icon6;
        icon6.addFile(QString::fromUtf8(":/Resources/icons/merge.png"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
        mergeButton->setIcon(icon6);
        mergeButton->setIconSize(QSize(22, 22));
        mergeButton->setCheckable(true);

        verticalLayout->addWidget(mergeButton);

        unmergeButton = new QToolButton(widget);
        unmergeButton->setObjectName("unmergeButton");
        unmergeButton->setMinimumSize(QSize(30, 30));
        unmergeButton->setMaximumSize(QSize(30, 30));
        QIcon icon7;
        icon7.addFile(QString::fromUtf8(":/Resources/icons/unmerge.png"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
        unmergeButton->setIcon(icon7);
        unmergeButton->setIconSize(QSize(22, 22));
        unmergeButton->setCheckable(true);

        verticalLayout->addWidget(unmergeButton);

        verticalSpacer_2 = new QSpacerItem(20, 40, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        verticalLayout->addItem(verticalSpacer_2);

        showMapButton = new QToolButton(widget);
        showMapButton->setObjectName("showMapButton");
        showMapButton->setMinimumSize(QSize(30, 30));
        showMapButton->setMaximumSize(QSize(30, 30));
        showMapButton->setIcon(icon);
        showMapButton->setIconSize(QSize(22, 22));
        showMapButton->setCheckable(true);
        showMapButton->setChecked(true);

        verticalLayout->addWidget(showMapButton);

        showAnimationButton = new QToolButton(widget);
        showAnimationButton->setObjectName("showAnimationButton");
        showAnimationButton->setMinimumSize(QSize(30, 30));
        showAnimationButton->setMaximumSize(QSize(30, 30));
        QIcon icon8;
        icon8.addFile(QString::fromUtf8(":/Resources/icons/animation.png"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
        showAnimationButton->setIcon(icon8);
        showAnimationButton->setIconSize(QSize(22, 22));
        showAnimationButton->setCheckable(true);
        showAnimationButton->setChecked(false);

        verticalLayout->addWidget(showAnimationButton);

        exportButton = new QToolButton(widget);
        exportButton->setObjectName("exportButton");
        exportButton->setMinimumSize(QSize(30, 30));
        exportButton->setMaximumSize(QSize(30, 30));
        QIcon icon9;
        icon9.addFile(QString::fromUtf8(":/Resources/icons/export.png"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
        exportButton->setIcon(icon9);
        exportButton->setIconSize(QSize(22, 22));

        verticalLayout->addWidget(exportButton);

        exploreButton = new QToolButton(widget);
        exploreButton->setObjectName("exploreButton");
        QIcon icon10;
        icon10.addFile(QString::fromUtf8(":/Resources/icons/explore.png"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
        exploreButton->setIcon(icon10);
        exploreButton->setIconSize(QSize(22, 22));

        verticalLayout->addWidget(exploreButton);

        verticalSpacer = new QSpacerItem(13, 231, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        verticalLayout->addItem(verticalSpacer);


        horizontalLayout->addWidget(widget);

        splitter->addWidget(layoutWidget);
        tabWidget = new QTabWidget(splitter);
        tabWidget->setObjectName("tabWidget");
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

        verticalLayout_2->addWidget(splitter);

        verticalLayout_2->setStretch(1, 1);

        retranslateUi(ViewWidget);

        tabWidget->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(ViewWidget);
    } // setupUi

    void retranslateUi(QWidget *ViewWidget)
    {
        ViewWidget->setWindowTitle(QString());
        syncButton->setText(QCoreApplication::translate("ViewWidget", "...", nullptr));
#if QT_CONFIG(tooltip)
        pickupModeButton->setToolTip(QCoreApplication::translate("ViewWidget", "Trips with pickup in this region.", nullptr));
#endif // QT_CONFIG(tooltip)
        pickupModeButton->setText(QString());
#if QT_CONFIG(tooltip)
        dropoffModeButton->setToolTip(QCoreApplication::translate("ViewWidget", "Trips with dropoff in this region.", nullptr));
#endif // QT_CONFIG(tooltip)
        dropoffModeButton->setText(QString());
#if QT_CONFIG(tooltip)
        pickupAndDropoffButton->setToolTip(QCoreApplication::translate("ViewWidget", "Trips with pickup and dropoff in this region.", nullptr));
#endif // QT_CONFIG(tooltip)
        pickupAndDropoffButton->setText(QString());
#if QT_CONFIG(tooltip)
        linkModeButton->setToolTip(QCoreApplication::translate("ViewWidget", "Trips from  first regions to second region.", nullptr));
#endif // QT_CONFIG(tooltip)
        linkModeButton->setText(QString());
        mergeButton->setText(QString());
        unmergeButton->setText(QString());
        showMapButton->setText(QCoreApplication::translate("ViewWidget", "...", nullptr));
#if QT_CONFIG(tooltip)
        showAnimationButton->setToolTip(QCoreApplication::translate("ViewWidget", "Show animation for current selection.", nullptr));
#endif // QT_CONFIG(tooltip)
        showAnimationButton->setText(QString());
        exportButton->setText(QString());
        exploreButton->setText(QCoreApplication::translate("ViewWidget", "explore", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(timeSeriesWidget), QCoreApplication::translate("ViewWidget", "Time Series", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(histogramWidget), QCoreApplication::translate("ViewWidget", "Histogram", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(scatterPlotWidget), QCoreApplication::translate("ViewWidget", "ScatterPlot", nullptr));
    } // retranslateUi

};

namespace Ui {
    class ViewWidget: public Ui_ViewWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_VIEWWIDGET_H
