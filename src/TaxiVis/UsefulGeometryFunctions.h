#ifndef USEFULGEOMETRYFUNCTIONS_H
#define USEFULGEOMETRYFUNCTIONS_H

#include <QPointF>
#include <QPainter>
#include <QPolygonF>
#include <QBrush>
#include <qmath.h>
namespace UsefulGeometry{
inline qreal dotProduct(const QPointF &p1, const QPointF &p2)
//!< Calculates dot product for vectors \c p1 and \c p2.
{
    return (p1.x()*p2.x()+p1.y()*p2.y());
}

inline qreal cosine(const QPointF &p1, const QPointF &p2)
//! Calculates qFastCos(angle) between vectors \c p1 and \c p2.
{
    return (dotProduct(p1,p2)/sqrt(dotProduct(p1,p1)*dotProduct(p2,p2)));
}

inline qreal distance(const QPointF &p1, const QPointF &p2)
//! Calculates r = |p1-p2|.
{
    return qSqrt(pow(p1.x()-p2.x(),2)+pow(p1.y()-p2.y(),2));
}

inline qreal length(const QPointF &p)
//! Calculates length of the vector r = |p|.
{
    return distance(p, QPointF(0,0));
}

inline qreal hillFunction(qreal sigma, qreal sigma_av)
//! Hill function \frac{sigma^2}{sigma^2+sigma_{av}^2}.
{
    return (sigma*sigma)/(sigma*sigma + sigma_av*sigma_av);
}

inline void drawArrow(QPainter& painter, const QPointF& p1, const QPointF& p2, qreal angle = M_PI/6)
//! Draws an arrow :)
{
    QPointF p = (p2-p1);
    qreal tangent = atan2(p.y(), p.x());
    qreal l = sqrt(pow(p.x(),2)+pow(p.y(),2))/3;

    QBrush oldBrush = painter.brush();
    painter.setBrush(QColor(painter.pen().color()));

    painter.drawLine(p1, p2);

    QPolygonF arrow;
    arrow << p2;
    arrow << QPointF(-l*qFastCos(tangent+angle) + p2.x(), -l*qFastSin(tangent+angle) + p2.y());
    arrow << p2-p/6;
    arrow << QPointF(-l*qFastCos(tangent-angle) + p2.x(), -l*qFastSin(tangent-angle) + p2.y());
    arrow << p2;

    painter.drawPolygon(arrow);

    painter.setBrush(oldBrush);
}

inline QPointF circumcenter(const QPointF &A, const QPointF &B, const QPointF &C)
//! Calculates circumcenter of triangle
{
    /*
     *       @ C
     *      / \
     *     / . \
     *    /   O \
     *  A@-------@B
     */

    qreal a = distance(B,C);
    qreal b = distance(A,C);
    qreal c = distance(B,A);

    QPointF O(a*A.x()+b*B.x()+c*C.x(),
              a*A.y()+b*B.y()+c*C.y());
    O /= a+b+c;

    return O;
}

inline qreal area(const QPointF &A, const QPointF &B, const QPointF &C)
//! Calculates area of triangle
{
    //Area is 0.5*|(B-A)x(C-A)|

    return 0.5 * qAbs((C-A).x() * (B-A).y() - (C-A).y() * (B-A).x());
}
};

inline void updateBounds(float& minValue,float& maxValue, float newValue){
    if(newValue < minValue)
        minValue = newValue;
    if(newValue > maxValue)
        maxValue = newValue;
}

inline void updateBounds(std::pair<float,float>& bounds, float newValue){
    float& minValue = bounds.first;
    float& maxValue = bounds.second;
    if(newValue < minValue)
        minValue = newValue;
    if(newValue > maxValue)
        maxValue = newValue;
}

#endif // USEFULGEOMETRYFUNCTIONS_H
