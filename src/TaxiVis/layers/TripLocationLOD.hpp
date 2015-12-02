#ifndef TRIP_LOCATION_LOD_HPP
#define TRIP_LOCATION_LOD_HPP
#include "TripLocation.hpp"

class GeographicalViewWidget;

class TripLocationLOD : public TripLocation
{
  Q_OBJECT
public:
  TripLocationLOD(GeographicalViewWidget *mw);

  bool lodEnabled();
  void setLodEnabled(bool b);

protected:
  void buildLocations();
  void renderGL();

  bool lod;
};

#endif
