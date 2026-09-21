/********************************************************************************
** Form generated from reading UI file 'TripAnimationConfig.ui'
**
** Created by: Qt User Interface Compiler version 6.11.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_TRIPANIMATIONCONFIG_H
#define UI_TRIPANIMATIONCONFIG_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QSlider>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_TripAnimationConfig
{
public:
    QGridLayout *gridLayout;
    QLabel *label_3;
    QLabel *label;
    QLabel *label_2;
    QSlider *glyphSizeSlider;
    QSlider *trailingPeriodSlider;
    QSlider *speedSlider;

    void setupUi(QWidget *TripAnimationConfig)
    {
        if (TripAnimationConfig->objectName().isEmpty())
            TripAnimationConfig->setObjectName("TripAnimationConfig");
        TripAnimationConfig->resize(320, 79);
        TripAnimationConfig->setStyleSheet(QString::fromUtf8("background: transparent;"));
        gridLayout = new QGridLayout(TripAnimationConfig);
        gridLayout->setObjectName("gridLayout");
        gridLayout->setContentsMargins(0, 0, 10, 0);
        label_3 = new QLabel(TripAnimationConfig);
        label_3->setObjectName("label_3");

        gridLayout->addWidget(label_3, 2, 0, 1, 1);

        label = new QLabel(TripAnimationConfig);
        label->setObjectName("label");

        gridLayout->addWidget(label, 0, 0, 1, 1);

        label_2 = new QLabel(TripAnimationConfig);
        label_2->setObjectName("label_2");

        gridLayout->addWidget(label_2, 1, 0, 1, 1);

        glyphSizeSlider = new QSlider(TripAnimationConfig);
        glyphSizeSlider->setObjectName("glyphSizeSlider");
        glyphSizeSlider->setMaximum(40);
        glyphSizeSlider->setOrientation(Qt::Horizontal);

        gridLayout->addWidget(glyphSizeSlider, 0, 1, 1, 1);

        trailingPeriodSlider = new QSlider(TripAnimationConfig);
        trailingPeriodSlider->setObjectName("trailingPeriodSlider");
        trailingPeriodSlider->setMinimum(1);
        trailingPeriodSlider->setMaximum(1000);
        trailingPeriodSlider->setOrientation(Qt::Horizontal);

        gridLayout->addWidget(trailingPeriodSlider, 1, 1, 1, 1);

        speedSlider = new QSlider(TripAnimationConfig);
        speedSlider->setObjectName("speedSlider");
        speedSlider->setMinimum(1);
        speedSlider->setMaximum(30);
        speedSlider->setOrientation(Qt::Horizontal);

        gridLayout->addWidget(speedSlider, 2, 1, 1, 1);


        retranslateUi(TripAnimationConfig);

        QMetaObject::connectSlotsByName(TripAnimationConfig);
    } // setupUi

    void retranslateUi(QWidget *TripAnimationConfig)
    {
        TripAnimationConfig->setWindowTitle(QCoreApplication::translate("TripAnimationConfig", "Form", nullptr));
        label_3->setText(QCoreApplication::translate("TripAnimationConfig", "Play Speed", nullptr));
        label->setText(QCoreApplication::translate("TripAnimationConfig", "Glyph Size", nullptr));
        label_2->setText(QCoreApplication::translate("TripAnimationConfig", "Tail Length", nullptr));
    } // retranslateUi

};

namespace Ui {
    class TripAnimationConfig: public Ui_TripAnimationConfig {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_TRIPANIMATIONCONFIG_H
