#include "mainwindow.h"
#include "ui_mainwindow.h"
//
#include "global.h"
#include "GroupRepository.h"
#include "viewwidget.h"

#include <QtGui>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //
    GroupRepository::getInstance();

    //
    connect( ui->actionAddMap, SIGNAL( triggered() ),
             this, SLOT( addNewMap() ) );

    // move( 0, 0 );
    // QWidget::showMaximized();

    // for to start with one map
    addNewMap();
    ui->mdiArea->subWindowList().front()->showMaximized();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::selectionChanged(){
    //emit updateSelectedTrips();
}

void MainWindow::addNewMap() {
  ViewWidget *vw = new ViewWidget( this );
  QMdiSubWindow *sw = ui->mdiArea->addSubWindow( vw );
  sw->show();

  ui->mdiArea->tileSubWindows();
}
