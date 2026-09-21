#ifndef TRIP_LOCATION_HPP
#define TRIP_LOCATION_HPP
#include "RenderingLayer.hpp"
#include <vector>
#include "AsyncTask.hpp"

class GeographicalViewWidget;

class TripLocation : public QObject, public RenderingLayer
{
  Q_OBJECT
public:
  TripLocation(GeographicalViewWidget *mw);
  virtual ~TripLocation();

  void cancelComputation() { locationJob.cancel(); dataReady = false; }
  bool computationBusy() const { return locationJob.isBusy(); }
  QColor pickupColor();
  void   setPickupColor(QColor color);
  QColor dropoffColor();
  void   setDropoffColor(QColor color);

  void initGL();
  void setEnabled(bool r);
  void render(QPainter *painter);

public slots:
  void updateData();
  
protected:
  virtual void buildLocations();
  virtual void renderGL();

  LatestTask<std::vector<float>> locationJob;
  bool                    dataReady;
  bool                    bufferDirty;
  GLBuffer                glBuffer;
  std::vector<float>      vertices;
  PQOpenGLShaderProgram       shader;
  QColor                  colorPickup;
  QColor                  colorDropoff;
  GeographicalViewWidget *geoWidget;
};

#endif
