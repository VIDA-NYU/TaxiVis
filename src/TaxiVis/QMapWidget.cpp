#include "QMapWidget.hpp"
#include "QMapView.hpp"
#include <QtCore/QTimer>
#include <QtGui/QScrollBar>
#include <QtGui/QGraphicsScene>
#include <QtGui/QSlider>
#include <QtGui/QApplication>
#include <QtOpenGL/QGLWidget>
#include <stdio.h>
#include <math.h>

QMapWidget::QMapWidget(QWidget *parent)
    : QGraphicsView(parent)
{
  this->initWidget(QPointF(40.7566, -73.9863), 15);
}

QMapWidget::QMapWidget(QPointF coords, int level, QWidget *parent)
    : QGraphicsView(parent)
{
  this->initWidget(coords, level);
}

void QMapWidget::initWidget(QPointF coords, int level)
{
  QGLFormat glf = QGLFormat::defaultFormat();
  glf.setSampleBuffers(true); 
  glf.setSamples(4);

  this->setViewport(new QGLWidget(glf));
  this->setFrameStyle(QFrame::NoFrame);
  this->setInteractive(true);
  
  this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  
  QGraphicsScene *scene = new QGraphicsScene(this);
  this->setScene(scene);
  
  this->mView = new QMapView(coords, level, this);
  scene->addItem(this->mView);
  this->mView->setResizesToContents(false);
  this->mView->setFocus();
  this->mView->settings()->globalSettings()->setAttribute(QWebSettings::PluginsEnabled, true);
}

void QMapWidget::resizeEvent(QResizeEvent *event)
{
  this->mView->setGeometry(QRectF(0, 0, this->width(), this->height()));
  this->centerOn(0.5*this->width(), 0.5*this->height());
  QGraphicsView::resizeEvent(event);
}

void QMapWidget::closeEvent(QCloseEvent *event)
{
  this->setViewport(NULL);
  QGraphicsView::closeEvent(event);
}

void QMapWidget::repaintContents()
{
  this->viewport()->update();
}
