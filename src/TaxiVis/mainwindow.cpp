#include "mainwindow.h"
#include "ui_mainwindow.h"
//
#include "global.h"
#include "GroupRepository.h"
#include "viewwidget.h"

#include <QtGui>
#include <QMdiSubWindow>

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

    // The designer default (910x634) is too short for the map and the plot
    // panel to coexist on a modern display. Size relative to the screen that
    // is actually available and centre the window.
    if (const QScreen *screen = QGuiApplication::primaryScreen()) {
        const QRect available = screen->availableGeometry();
        resize(qMin(1500, int(available.width()  * 0.85)),
               qMin(980,  int(available.height() * 0.85)));
        move(available.center() - QPoint(width()/2, height()/2));
    }

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
  sw->setAttribute(Qt::WA_DeleteOnClose);
  sw->show();

  ui->mdiArea->tileSubWindows();
}
