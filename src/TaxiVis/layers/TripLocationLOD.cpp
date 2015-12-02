#include "TripLocationLOD.hpp"
#include "QMapView.hpp"
#include "geographicalviewwidget.h"
#include "global.h"
#include <QtGui/QVector2D>
#include <QtOpenGL/QGLShaderProgram>
#include <queue>

TripLocationLOD::TripLocationLOD(GeographicalViewWidget *gw)
    : TripLocation(gw), lod(true)
{
}

struct Location {
  float pos[4];
  float pdis() const
  {
    return this->pos[0]*this->pos[0]+this->pos[1]*this->pos[1];
  }
  bool operator<(const Location& loc) const {  
    return (this->pdis()<loc.pdis());
  }
};

void progressiveLayout(Location *locations, int left, int right, float *pickup, float *dropoff)
{
  std::queue< std::pair<int,int> > q;
  q.push(std::make_pair(left,right));
  while (!q.empty()) {
    std::pair<int,int> bounds = q.front();
    q.pop();
    if (bounds.first<=bounds.second) {
      int mid = (bounds.first+bounds.second)/2;
      pickup[0] = locations[mid].pos[0];
      pickup[1] = locations[mid].pos[1];
      dropoff[0] = locations[mid].pos[2];
      dropoff[1] = locations[mid].pos[3];
      pickup  += 2;
      dropoff += 2;
      if (bounds.first<=mid-1)
        q.push(std::make_pair(bounds.first, mid-1));
      if (mid+1<=bounds.second)
        q.push(std::make_pair(mid+1, bounds.second));
    }
  }
}

void TripLocationLOD::buildLocations()
{
  KdTrip::TripSet::iterator it;
  KdTrip::TripSet *selectedTrips = this->geoWidget->getSelectedTrips();
  std::vector<Location> locations;
  locations.reserve(selectedTrips->size());
  for (it=selectedTrips->begin(); it!=selectedTrips->end(); it++) {
    const KdTrip::Trip *trip = *it;
    Location t;
    t.pos[0] = trip->pickup_lat;
    t.pos[1] = trip->pickup_long;
    t.pos[2] = trip->dropoff_lat;
    t.pos[3] = trip->dropoff_long;
    locations.push_back(t);
  }
  std::sort(locations.begin(), locations.end());
  this->vertices.clear();
  this->vertices.resize(4*locations.size());
  float *pickup = &this->vertices[0];
  float *dropoff = pickup + 2*locations.size();
  progressiveLayout(&locations[0], 0, locations.size()-1, pickup, dropoff);
  this->bufferDirty = true;
  this->dataReady = true;
}

#define USE_VBO 0

void TripLocationLOD::renderGL()
{
#if USE_VBO
  if (this->bufferDirty) {
    this->glBuffer.setData(GL_ARRAY_BUFFER, this->vertices.size()*sizeof(float), &this->vertices[0], GL_DYNAMIC_DRAW);
    this->bufferDirty = false;
  }
#endif  

  float zoomLevel = this->geoWidget->mapView()->zoomLevel();
  float pointSize = 7.0*std::min(2.0, std::max(0.4, exp2(zoomLevel-15.0)));
  int vCount = this->vertices.size()/4;
  if (this->lod) {
    // specify the most # points at the highest level, aka 23
    // and at least # points at any level
    const int maxPoint = 512*1024;
    const int minPoint = 256;
    vCount = std::max(std::min((int)(maxPoint*exp2(zoomLevel-23)), vCount),
                      std::min(vCount, minPoint));
  }

#if USE_VBO
  this->shader->bind();
#endif

  QSizeF size = this->geoWidget->mapView()->size();
  
#if USE_VBO
  this->shader->setUniformValue("zoom", zoomLevel);
  this->shader->setUniformValue("center", this->geoWidget->mapView()->center());
  this->shader->setUniformValue("size", QVector2D(size.width(), size.height()));
#endif
  
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  
#if USE_VBO
  glEnableClientState(GL_VERTEX_ARRAY);
  glBindBuffer(GL_ARRAY_BUFFER, this->glBuffer.buffer);
  glVertexPointer(2, GL_FLOAT, 0, 0);
#endif
  
  glPointSize(pointSize);

  glColor4f(this->colorPickup.redF(),
            this->colorPickup.greenF(),
            this->colorPickup.blueF(),
            this->colorPickup.alphaF());
#if USE_VBO
  glDrawArrays(GL_POINTS, 0, vCount);
#else
  glBegin(GL_POINTS);
  for (int i=0; i<vCount; i++) {
    QPointF p(this->geoWidget->mapView()->mapFromGeoLocation(QPointF(this->vertices[i*2], this->vertices[i*2+1])));
    glVertex2f(p.x(), p.y());
  }
  glEnd();
#endif
  
  glColor4f(this->colorDropoff.redF(),
            this->colorDropoff.greenF(),
            this->colorDropoff.blueF(),
            this->colorDropoff.alphaF());
#if USE_VBO
  glDrawArrays(GL_POINTS, this->vertices.size()/4, vCount);
#else
  glBegin(GL_POINTS);
  int shift = this->vertices.size()/4;
  for (int i=0; i<vCount; i++) {
    QPointF p(this->geoWidget->mapView()->mapFromGeoLocation(QPointF(this->vertices[(shift+i)*2], this->vertices[(shift+i)*2+1])));
    glVertex2f(p.x(), p.y());
  }
  glEnd();
#endif
  
  glDisableClientState(GL_VERTEX_ARRAY);    
  glDisable(GL_BLEND);
  
  this->shader->release();
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}

bool TripLocationLOD::lodEnabled()
{
  return this->lod;
}

void TripLocationLOD::setLodEnabled(bool b)
{
  this->lod = b;
}
