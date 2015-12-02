#include "Selection.h"
#include <sstream>
#include <cassert>
#include <iostream>
#include <cstdlib>
#include <sstream>
#include <cmath>
#include "UsefulGeometryFunctions.h"

using namespace std;

/**************
  * Selection *
  *************/

Selection::Selection():
    active(true),
    selectionType(Selection::START)
{}

Selection::Selection(const QPainterPath& path):
    active(true),
    selectionType(Selection::START){
    selectionGeometry = QPainterPath(path);
}

Selection::Selection(const QPainterPath &path, Selection::TYPE selectionType):
    active(true),
    selectionType(selectionType){
    selectionGeometry = QPainterPath(path);
}

Selection::~Selection(){
    //cout << "Selection Destructor" << endl;
}

bool Selection::isActive(){
    return active;
}

void Selection::setActive(bool x){
    active = x;
}

void Selection::toogleSetActive(){
    active = !active;
}

bool Selection::contains(QPointF p){
    return selectionGeometry.contains(p);
}

QPainterPath Selection::getGeometry(){
    return selectionGeometry;
}

void Selection::getCenter(QPointF& /*p*/){
    selectionGeometry.boundingRect().center();
}

void Selection::translate(const QPointF& v){
    selectionGeometry.translate(v);
}

QRectF Selection::boundingBox(){
    return selectionGeometry.boundingRect();
}

void Selection::scale(BoundingBoxCorner /*corner*/,QPointF /*center*/ , float /*dx*/, float /*dy*/){
}

void Selection::scaleBoundingBox(float , float , float , float ){}

Selection::BoundingBoxCorner Selection::getClosestCorner(float xx, float yy){
    QRectF bbox = selectionGeometry.boundingRect();
    float x,y,w,h;
    x = bbox.x();
    y = bbox.y();
    w = bbox.width();
    w = bbox.height();

    QPointF p(xx,yy);
    QPointF c0(x,y+h);
    QPointF c1(x,y);
    QPointF c2(x+w,y);
    QPointF c3(x+w,y+h);

    qreal d0 = UsefulGeometry::distance(p,c0);
    qreal d1 = UsefulGeometry::distance(p,c1);
    qreal d2 = UsefulGeometry::distance(p,c2);
    qreal d3 = UsefulGeometry::distance(p,c3);

    if (d0 <= d1 && d0 <= d2 && d0 <= d3)
    {
        //distance = d0;
        return Selection::UPPER_LEFT;
    }
    else if (d1 <= d2 && d1 <= d3)
    {
        //distance = d1;
        return Selection::LOWER_LEFT;
    }
    else if (d2 <= d3)
    {
        //distance = d2;
        return Selection::LOWER_RIGHT;
    }
    else
    {
        //distance = d3;
        return Selection::UPPER_RIGHT;
    }

}

string Selection::str(){
    return "";
}

Selection::TYPE Selection::getType(){
    return selectionType;
}

void Selection::setType(Selection::TYPE mode)
{
    selectionType = mode;
}

