#ifndef TRIP_LOCATION_HPP
#define TRIP_LOCATION_HPP
#include "RenderingLayer.hpp"
#include <vector>

class GeographicalViewWidget;

class TripLocation : public QObject, public RenderingLayer
{
  Q_OBJECT
public:
  TripLocation(GeographicalViewWidget *mw);
  virtual ~TripLocation();

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

  bool                    dataReady;
  bool                    bufferDirty;
  GLBuffer                glBuffer;
  std::vector<float>      vertices;
  PQGLShaderProgram       shader;
  QColor                  colorPickup;
  QColor                  colorDropoff;
  GeographicalViewWidget *geoWidget;
};

#endif
