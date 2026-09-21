/********************************************************************************
** Form generated from reading UI file 'TripAnimationToolBar.ui'
**
** Created by: Qt User Interface Compiler version 6.11.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_TRIPANIMATIONTOOLBAR_H
#define UI_TRIPANIMATIONTOOLBAR_H

#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QSlider>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_TripAnimationToolBar
{
public:
    QHBoxLayout *toolBarHLayout;
    QToolButton *configButton;
    QToolButton *frameButton;
    QToolButton *playButton;
    QToolButton *resetButton;
    QSpacerItem *horizontalSpacer_2;
    QSlider *frameSlider;
    QSpacerItem *horizontalSpacer;

    void setupUi(QWidget *TripAnimationToolBar)
    {
        if (TripAnimationToolBar->objectName().isEmpty())
            TripAnimationToolBar->setObjectName("TripAnimationToolBar");
        TripAnimationToolBar->resize(330, 33);
        QSizePolicy sizePolicy(QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Minimum);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(TripAnimationToolBar->sizePolicy().hasHeightForWidth());
        TripAnimationToolBar->setSizePolicy(sizePolicy);
        TripAnimationToolBar->setStyleSheet(QString::fromUtf8("background: transparent;"));
        toolBarHLayout = new QHBoxLayout(TripAnimationToolBar);
        toolBarHLayout->setSpacing(0);
        toolBarHLayout->setObjectName("toolBarHLayout");
        toolBarHLayout->setContentsMargins(0, 0, 10, 0);
        configButton = new QToolButton(TripAnimationToolBar);
        configButton->setObjectName("configButton");
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/Resources/icons/config.png"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
        icon.addFile(QString::fromUtf8(":/Resources/icons/config_on.png"), QSize(), QIcon::Mode::Normal, QIcon::State::On);
        configButton->setIcon(icon);
        configButton->setCheckable(true);

        toolBarHLayout->addWidget(configButton);

        frameButton = new QToolButton(TripAnimationToolBar);
        frameButton->setObjectName("frameButton");
        frameButton->setAutoFillBackground(false);
        QIcon icon1;
        icon1.addFile(QString::fromUtf8(":/Resources/icons/time.png"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
        icon1.addFile(QString::fromUtf8(":/Resources/icons/time_on.png"), QSize(), QIcon::Mode::Normal, QIcon::State::On);
        frameButton->setIcon(icon1);
        frameButton->setCheckable(true);

        toolBarHLayout->addWidget(frameButton);

        playButton = new QToolButton(TripAnimationToolBar);
        playButton->setObjectName("playButton");
        QIcon icon2;
        icon2.addFile(QString::fromUtf8(":/Resources/icons/play.png"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
        icon2.addFile(QString::fromUtf8(":/Resources/icons/pause.png"), QSize(), QIcon::Mode::Normal, QIcon::State::On);
        playButton->setIcon(icon2);
        playButton->setCheckable(true);

        toolBarHLayout->addWidget(playButton);

        resetButton = new QToolButton(TripAnimationToolBar);
        resetButton->setObjectName("resetButton");
        QIcon icon3;
        icon3.addFile(QString::fromUtf8(":/Resources/icons/reset.png"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
        resetButton->setIcon(icon3);

        toolBarHLayout->addWidget(resetButton);

        horizontalSpacer_2 = new QSpacerItem(20, 20, QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Minimum);

        toolBarHLayout->addItem(horizontalSpacer_2);

        frameSlider = new QSlider(TripAnimationToolBar);
        frameSlider->setObjectName("frameSlider");
        frameSlider->setOrientation(Qt::Horizontal);

        toolBarHLayout->addWidget(frameSlider);

        horizontalSpacer = new QSpacerItem(0, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        toolBarHLayout->addItem(horizontalSpacer);

        toolBarHLayout->setStretch(5, 1);

        retranslateUi(TripAnimationToolBar);

        QMetaObject::connectSlotsByName(TripAnimationToolBar);
    } // setupUi

    void retranslateUi(QWidget *TripAnimationToolBar)
    {
        TripAnimationToolBar->setWindowTitle(QCoreApplication::translate("TripAnimationToolBar", "Trip Animation ToolBar", nullptr));
        configButton->setText(QCoreApplication::translate("TripAnimationToolBar", "...", nullptr));
        frameButton->setText(QCoreApplication::translate("TripAnimationToolBar", "...", nullptr));
        playButton->setText(QCoreApplication::translate("TripAnimationToolBar", "...", nullptr));
        resetButton->setText(QCoreApplication::translate("TripAnimationToolBar", "...", nullptr));
    } // retranslateUi

};

namespace Ui {
    class TripAnimationToolBar: public Ui_TripAnimationToolBar {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_TRIPANIMATIONTOOLBAR_H
