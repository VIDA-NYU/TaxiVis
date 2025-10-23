#include <QApplication>
#include "global.h"
#include "mainwindow.h"

int main(int argc, char *argv[])
{
  QApplication a(argc, argv);
  Global::getInstance(); //loads data
  MainWindow w;
  w.resize(1280, 800);
  w.show();
  return a.exec();
}
