#ifndef NEIGHBORHOOD_H
#define NEIGHBORHOOD_H

#include <string>
#include <QPolygonF>

class Neighborhood
{
private:
    std::string     name;

public:
    Neighborhood();
    Neighborhood(std::string name, const QVector<QPointF>& g);

    std::string  getName();
    QRectF       getBBox();
    bool         contains(const QPointF &p);

    //
    QPolygonF       geometry;
};

#endif // NEIGHBORHOOD_H
