#ifndef NEIGHBORHOODSET_H
#define NEIGHBORHOODSET_H

#include <map>
#include "neighborhood.h"

class NeighborhoodSet
{
private:
    std::map<std::string,Neighborhood*> mapNamesToNeigh;
public:
    NeighborhoodSet();

    void          loadGeometry();
    Neighborhood* getByName(std::string name);
    Neighborhood* getNeighThatContains(const QPointF& geoLocation);
    int           getNumberOfNeighborhoods();

    void getIterators(std::map<std::string,Neighborhood*>::iterator& begin,
                      std::map<std::string,Neighborhood*>::iterator& end);
};

#endif // NEIGHBORHOODSET_H
