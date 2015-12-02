#include "neighborhood.h"

Neighborhood::Neighborhood(){

}

Neighborhood::Neighborhood(std::string name, const QVector<QPointF>& g):
    name(name){
    geometry = QPolygonF(g);
}

std::string  Neighborhood::getName(){
    return name;
}

QRectF Neighborhood::getBBox(){
    return geometry.boundingRect();
}

bool Neighborhood::contains(const QPointF &p){
    QRectF bbox = getBBox();
    if(!bbox.contains(p))
        return false;

    return geometry.contains(p);
}
