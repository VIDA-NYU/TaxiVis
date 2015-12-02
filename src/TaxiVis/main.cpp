#include <QtGui/QApplication>
#include "global.h"
#include "mainwindow.h"

int main(int argc, char *argv[])
{
  QApplication a(argc, argv);
  Global::getInstance(); //loads data
  MainWindow w;
  w.showMaximized();
  w.show();
  return a.exec();
}
