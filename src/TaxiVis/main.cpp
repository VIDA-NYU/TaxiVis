#include <QApplication>
#include <QDebug>
#include <exception>
#include "global.h"
#include "mainwindow.h"
#include "qcustomplot.h"

int main(int argc, char *argv[])
{
  QApplication a(argc, argv);
  qInfo() << "TaxiVis: Qt" << qVersion() << "QCustomPlot" << QCUSTOMPLOT_VERSION_STR;
  try {
    Global::getInstance(); // loads and validates data
  } catch (const std::exception &error) {
    qCritical() << "Unable to load TaxiVis data:" << error.what();
    return 1;
  }
  MainWindow w; // sizes and centres itself relative to the available screen
  w.show();
  return a.exec();
}
