#ifndef SELECTION_H
#define SELECTION_H

#include <string>
#include <vector>
#include <map>
#include <QPointF>
#include <QPainterPath>

class Selection
{
public:
    enum BoundingBoxCorner{
        LOWER_LEFT,LOWER_RIGHT,UPPER_LEFT,UPPER_RIGHT
    };

    enum TYPE{
        START=0, END, START_AND_END
    };

private:
    QPainterPath selectionGeometry;
protected:
    bool active;
    bool selected;
    TYPE selectionType;
public:
    Selection();
    Selection(const QPainterPath& path);
    Selection(const QPainterPath& path, TYPE selectionType);
    ~Selection();
    bool contains(QPointF p);
    QPainterPath getGeometry();
    void getCenter(QPointF&);
    void translate(const QPointF& v);
    QRectF boundingBox();
    void scaleBoundingBox(float x, float y, float w, float h);
    std::string str();
    TYPE getType();
    void setType(TYPE mode);

    BoundingBoxCorner getClosestCorner(float x, float y);
    void              scale(BoundingBoxCorner,QPointF center, float dx, float dy);
    void              setActive(bool);
    void              toogleSetActive();
    bool              isActive();
};

#endif // SELECTION_H
