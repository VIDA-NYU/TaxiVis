#include "neighborhoodgraph.h"
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <cassert>
#include <limits.h>

using namespace std;

NeighborhoodGraph::NeighborhoodGraph(int numberOfNodes):
    numberOfNodes(numberOfNodes){
    edges.resize(numberOfNodes * numberOfNodes);
}

NeighborhoodGraph::NeighborhoodGraph(const std::vector<float> &e, int numberOfNodes):
    numberOfNodes(numberOfNodes){
    edges = e;
}

void NeighborhoodGraph::getBoundWeights(float &minWeight, float &maxWeight){
    minWeight = INT_MAX;
    maxWeight = INT_MIN;

    int size = edges.size();

    for(int i = 0 ; i < size ; ++i){
        if(edges[i] < minWeight)
            minWeight = edges[i];
        if(edges[i] > maxWeight)
            maxWeight = edges[i];
    }
}

float NeighborhoodGraph::getEdge(int originIndex, int destinationIndex){
    int linearIndex = originIndex * numberOfNodes + destinationIndex;
    int size = edges.size();
    assert(0 <= linearIndex && linearIndex < size);
    return edges[linearIndex];
}

void NeighborhoodGraph::setEdges(float v){
    int size = edges.size();
    for(int i = 0 ; i < size ; ++i){
        edges[i] = v;
    }
}

void NeighborhoodGraph::setEdges(const vector<float> &e){
    //cout << "e.size() = " << e.size() << " edges.size() " << edges.size() << endl;
    assert(e.size() == edges.size());
    edges = e;
}
