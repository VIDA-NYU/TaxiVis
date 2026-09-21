#include <GL/glew.h>
#include "TripLocation.hpp"
#include "QMapTileWidget.hpp"
#include "geographicalviewwidget.h"
#include "global.h"
#include <QVector2D>
#include <QOpenGLShaderProgram>

// Qt's OpenGL headers undef GLEW macros, so we need to redefine them
#ifndef glBindBuffer
#define glBindBuffer __glewBindBuffer
#endif

TripLocation::TripLocation(GeographicalViewWidget *gw) :
    dataReady(false),
    bufferDirty(false),
    colorPickup(26, 77, 128, 204),
    colorDropoff(255, 51, 26, 204),
    geoWidget(gw)
{
}

TripLocation::~TripLocation()
{
}

void TripLocation::setEnabled(bool r)
{
  if (this->enabled!=r) {
    this->enabled = r;
    if (this->enabled && !this->dataReady)
        this->buildLocations();
  }
}

QColor TripLocation::pickupColor()
{
  return this->colorPickup;
}

void TripLocation::setPickupColor(QColor color)
{
  this->colorPickup = color;
}

QColor TripLocation::dropoffColor()
{
  return this->colorDropoff;
}

void TripLocation::setDropoffColor(QColor color)
{
  this->colorDropoff = color;
}

void TripLocation::render(QPainter *painter)
{
  if (this->enabled && this->dataReady) {
    this->useGLPainting(painter);
    this->renderGL();
  }
}

void TripLocation::updateData()
{
  locationJob.cancel();
  this->dataReady = false;
  if (this->enabled)
    this->buildLocations();  
}

void TripLocation::initGL()
{
  glewInit();
  this->glBuffer.generate();
  const QOpenGLContext *context = QOpenGLContext::currentContext();
  if (context) {
    this->shader = PQOpenGLShaderProgram(new QOpenGLShaderProgram());
    this->shader->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/Resources/shaders/location.120.vert");
    this->shader->link();
  }
}

void TripLocation::buildLocations()
{
  if (!geoWidget->getSelectedTrips() || !geoWidget->hasCurrentSelection()) return;
  const auto trips = *geoWidget->getSelectedTrips();
  locationJob.submit([trips](const Cancellation &cancel) {
      std::vector<float> result(4*trips.size());
      size_t i=0;
      for (auto trip : trips) {
          if ((i & 1023)==0) cancel.check();
          result[2*i]=trip->pickup_lat; result[2*i+1]=trip->pickup_long;
          result[2*trips.size()+2*i]=trip->dropoff_lat; result[2*trips.size()+2*i+1]=trip->dropoff_long;
          ++i;
      }
      return result;
  }, [this](std::vector<float> result) {
      vertices=std::move(result); bufferDirty=true; dataReady=true;
      geoWidget->repaintContents();
  });
}

void TripLocation::renderGL()
{
  if (this->bufferDirty) {
    this->glBuffer.setData(GL_ARRAY_BUFFER, this->vertices.size()*sizeof(float), this->vertices.data(), GL_DYNAMIC_DRAW);
    this->bufferDirty = false;
  }

  this->shader->bind();

  QSizeF size = this->geoWidget->mapView()->size();
  this->shader->setUniformValue("zoom", (float)this->geoWidget->mapView()->zoomLevel());
  this->shader->setUniformValue("center", this->geoWidget->mapView()->center());
  this->shader->setUniformValue("size", QVector2D(size.width(), size.height()));
  
  glEnable(GL_POINT_SMOOTH);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnableClientState(GL_VERTEX_ARRAY);
 
  glBindBuffer(GL_ARRAY_BUFFER, this->glBuffer.buffer);
  glVertexPointer(2, GL_FLOAT, 0, 0);

  float pointSize = 10.0*std::min(2.0, std::max(0.4, exp2(this->geoWidget->mapView()->zoomLevel()-15.0)));
  glPointSize(pointSize);
  
  glColor4f(this->colorPickup.redF(),
            this->colorPickup.greenF(),
            this->colorPickup.blueF(),
            this->colorPickup.alphaF());
  glDrawArrays(GL_POINTS, 0, this->vertices.size()/4);
  
  glColor4f(this->colorDropoff.redF(),
            this->colorDropoff.greenF(),
            this->colorDropoff.blueF(),
            this->colorDropoff.alphaF());
  glDrawArrays(GL_POINTS, this->vertices.size()/4, this->vertices.size()/4);
  
  glDisableClientState(GL_VERTEX_ARRAY);    
  glDisable(GL_BLEND);
  
  this->shader->release();
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}
