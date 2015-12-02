#include "neighborhoodset.h"
#include <iostream>
#include <cassert>
#include <fstream>

using namespace std;

#define xDEBUG

NeighborhoodSet::NeighborhoodSet(){
}

void NeighborhoodSet::loadGeometry(){

    ifstream neighGeoFile((std::string(DATA_DIR)+"neighborhoodsGeometry.txt").c_str());
    string line;
    int count = 0;
    while(getline(neighGeoFile,line)){
        //cout << "Entering " << count++ << endl;
        if(line.size() <= 1)
            continue;

        //parse name line
        size_t pos = line.find_first_of(';');
        string neighName = line.substr(0,pos);
        int numPoints = atoi(line.substr(pos+1).c_str());
        QVector<QPointF> neighGeo;
        //cout << "Loading " << neighName << " " << numPoints << endl;
        for(int i = 0 ; i < numPoints ; ++i){
            float lat, lng;
            neighGeoFile >> lng >> lat;
            //cout << "   " << lat << " " << lng << endl;
            QPointF p(lat,lng);
            neighGeo.push_back(p);
        }
        Neighborhood *neigh = new Neighborhood(neighName,neighGeo);
        mapNamesToNeigh[neighName] = neigh;
    }
}

Neighborhood *NeighborhoodSet::getByName(string name){
    assert(mapNamesToNeigh.count(name) > 0);
    return mapNamesToNeigh[name];
}

Neighborhood* NeighborhoodSet::getNeighThatContains(const QPointF& p){
    Neighborhood* container = NULL;
    map<string,Neighborhood*>::iterator it;

    for (it = mapNamesToNeigh.begin() ; it != mapNamesToNeigh.end() ; ++it){
        //cout << "   Testing " << it->first << endl;
        Neighborhood* neighborhood           = it->second;

        if (neighborhood->contains(p)){
            container = neighborhood;
            break;
        }
    }

    return container;
}

int NeighborhoodSet::getNumberOfNeighborhoods(){
    return mapNamesToNeigh.size();
}

void NeighborhoodSet::getIterators(std::map<std::string,Neighborhood*>::iterator& begin,
                                   std::map<std::string,Neighborhood*>::iterator& end){
    begin = mapNamesToNeigh.begin();
    end   = mapNamesToNeigh.end();
}
