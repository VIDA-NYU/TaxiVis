#include "timeselectionwidget.h"
#include "ui_timeselectionwidget.h"
#include <QMessageBox>
#include <iostream>
#include <QDebug>
#include <QtGui/QInputDialog>
#include <cassert>

using namespace std;

TimeSelectionWidget::TimeSelectionWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TimeSelectionWidget)
{
    ui->setupUi(this);

    //
    uint x = 9668;
    ui->stepBackButton->setText(QString::fromUcs4(&x,1));

    x = 9658;
    ui->stepForwardButton->setText(QString::fromUcs4(&x,1));

   //
    connect(ui->queryButton,SIGNAL(clicked()),this,SLOT(timeDialogsChanged()));
   //
    connect(ui->exploreButton,SIGNAL(clicked()),this,SLOT(exploreTime()));

}

TimeSelectionWidget::~TimeSelectionWidget()
{
    delete ui;
}

int TimeSelectionWidget::getStepSize(){

    int stepSizeInMinutes = -1;
    QString selectedText = ui->comboBox_2->currentText();

    if(!selectedText.compare(QString::fromAscii("15 min"))){
        stepSizeInMinutes = 15;
    }
    else if(!selectedText.compare(QString::fromAscii("30 min"))){
        stepSizeInMinutes = 30;
    }
    else if(!selectedText.compare(QString::fromAscii("1 hour"))){
        stepSizeInMinutes = 60;
    }
    else if(!selectedText.compare(QString::fromAscii("1 day"))){
        stepSizeInMinutes = 24*60;
    }
    else if(!selectedText.compare(QString::fromAscii("1 week"))){
        stepSizeInMinutes = 24*60*7;
    }

    assert(stepSizeInMinutes != -1);//make sure that one was selected

    return stepSizeInMinutes;
}

QDateTime TimeSelectionWidget::getStartTime(){
    return ui->startTime->dateTime();
}

QDateTime TimeSelectionWidget::getEndTime(){
    return ui->endTime->dateTime();
}

void TimeSelectionWidget::setTimes(QDateTime start, QDateTime end){
    ui->startTime->setDateTime(start);
    ui->endTime->setDateTime(end);
}

void TimeSelectionWidget::stepBack(){
    int stepSize = getStepSize();

    QDateTime startDate = ui->startTime->dateTime();
    startDate = startDate.addSecs(-stepSize*60);
    ui->startTime->setDateTime(startDate);
    QDateTime endDate = ui->endTime->dateTime();
    endDate = endDate.addSecs(-stepSize*60);
    ui->endTime->setDateTime(endDate);

    timeDialogsChanged();
}

void TimeSelectionWidget::stepForward(){
    int stepSize = getStepSize();

    QDateTime startDate = ui->startTime->dateTime();
    startDate = startDate.addSecs(stepSize*60);
    ui->startTime->setDateTime(startDate);
    QDateTime endDate = ui->endTime->dateTime();
    endDate = endDate.addSecs(stepSize*60);
    ui->endTime->setDateTime(endDate);

    timeDialogsChanged();
}

void TimeSelectionWidget::timeDialogsChanged(){
  if (this->ui->tabWidget->currentWidget()==this->ui->regularTimeWidget) {
    if(ui->startTime->dateTime() > ui->endTime->dateTime()){
        QMessageBox::critical(this, QString::fromAscii("Error"),QString::fromAscii("Start time should be smaller than End Time!"));
    }
    else{
        //qDebug() << "timeDialogsChanged " << getStartTime() << "; " << getEndTime();
        emit timeUpdated(getStartTime(),getEndTime());
    }
  }
  else if (this->ui->tabWidget->currentWidget()==this->ui->recurrentTimeWidget) {
    emit recurrentTimeUpdated(this->ui->recurrentTimeWidget);
  }
}

void TimeSelectionWidget::exploreTime()
{
  if (this->ui->tabWidget->currentWidget()==this->ui->regularTimeWidget) {
    bool ok;
    int defaultValue[] = {4, 2, 2, 7, 4};
    int steps = QInputDialog::getInt(this, "Parameter Exploration",
                                     QString("Specify the number of '%1' steps").arg(this->ui->comboBox_2->currentText()),
                                     defaultValue[this->ui->comboBox_2->currentIndex()], 2, 12, 1, &ok);
    if (ok) {
      uint stepSizes[] = {15, 30, 60, 24*60, 7*24*60};
      uint delta = stepSizes[this->ui->comboBox_2->currentIndex()]*60;
      DateTimeList timeRanges;
      QDateTime start = this->getStartTime();
      QDateTime end = this->getEndTime();
      for (int i=0; i<steps; i++)
        timeRanges.append(qMakePair(start.addSecs(i*delta), end.addSecs(i*delta)));
      emit exploreInTime(timeRanges);
    }
  }
  else if (this->ui->tabWidget->currentWidget()==this->ui->recurrentTimeWidget) {
    emit exploreInTime(this->ui->recurrentTimeWidget->getSelectedRanges());
  }
}
