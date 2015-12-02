#ifndef NEIGHBORHOODGRAPH_H
#define NEIGHBORHOODGRAPH_H

#include <vector>

class NeighborhoodGraph
{
public:
    std::vector<float> edges;
    int numberOfNodes;

public:
    NeighborhoodGraph(int numberOfNodes);
    NeighborhoodGraph(const std::vector<float> &e, int numberOfNodes);
    void getBoundWeights(float &minWeight, float &maxWeight);
    float getEdge(int originIndex, int destinationIndex);
    void setEdges(float);
    void  setEdges(const std::vector<float> &e);
};

#endif // NEIGHBORHOODGRAPH_H
