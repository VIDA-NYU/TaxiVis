#include "geographicalviewwidget.h"
#include "viewwidget.h"
#include "QMapView.hpp"
#include "global.h"
#include "GroupRepository.h"
#include "coordinator.h"
#include "layers/GridMap.hpp"
#include "layers/HeatMap.hpp"
#include "layers/TripAnimation.hpp"
#include "layers/TripLocation.hpp"
#include "layers/TripLocationLOD.hpp"
#include "util/colorbar.h"
#include "UsefulGeometryFunctions.h"

#include <iostream>
#include <cmath>

#include <QGraphicsSceneMouseEvent>
#include <QDebug>
#include <QEventLoop>
#include <QGraphicsSceneMouseEvent>
#include <QtGui/QProgressDialog>

#include "CityMap.hpp"

using namespace std;

GeographicalViewWidget::GeographicalViewWidget(QWidget *parent) :
    QMapWidget(parent),
    currentState(GeographicalViewWidget::IDLE),
    basePosition(-1,-1),
    selectedTrips(NULL),
    renderTrips(true),
    selectionType(Selection::START),
    selectionMode(SINGLE),
    selectionTimeVisible(false),
    selectionTimeColor(Qt::blue),
    queryDescriptionVisible(false)
{
    this->setCoordinator(Coordinator::instance());
  
    this->selectionTimes = DateTimeList();
    this->selectionTimes.append(DateTimePair(QDateTime(), QDateTime()));
  
    this->layerHeatMap = new HeatMap(this);
    this->mapView()->addRenderingLayer(this->layerHeatMap);
    this->connect(this, SIGNAL(datasetUpdated()), this->layerHeatMap, SLOT(updateData()));
    
    this->layerLocation = new TripLocationLOD(this);
    this->mapView()->addRenderingLayer(this->layerLocation);
    this->connect(this, SIGNAL(datasetUpdated()), this->layerLocation, SLOT(updateData()));

    this->layerNeighborhood = new NumTripsGridMap(this);
    this->mapView()->addRenderingLayer(this->layerNeighborhood);
    this->layerNeighborhood->loadGrid(QString(DATA_DIR)+"neighborhoods.txt");
    this->connect(this, SIGNAL(datasetUpdated()), this->layerNeighborhood, SLOT(updateData()));

    this->layerZipCode = new PickupDropoffGridMap(this);//FarePerMileGridMap(this);
    this->layerZipCode->setColorScale(ColorScaleFactory::getInstance(SEQUENTIAL_SINGLE_HUE_RED));
    this->mapView()->addRenderingLayer(this->layerZipCode);
    this->layerZipCode->loadGrid(QString(DATA_DIR)+"neighborhoods.txt"); //"zipcodes.txt"
    this->connect(this, SIGNAL(datasetUpdated()), this->layerZipCode, SLOT(updateData()));

    this->layerAnimation = new TripAnimation(this);
    this->mapView()->addRenderingLayer(this->layerAnimation);
    this->connect(this, SIGNAL(datasetUpdated()), this->layerAnimation, SLOT(updateData()));
    
    //
    this->colorbar = new ColorBar;
}

GeographicalViewWidget::~GeographicalViewWidget()
{
    delete this->layerLocation;
    delete this->layerZipCode;
    delete this->layerNeighborhood;
    delete this->layerAnimation;
    delete this->colorbar;
}

void GeographicalViewWidget::setSelectedTripsRepository(KdTrip::TripSet *v){
    selectedTrips = v;
}

void GeographicalViewWidget::setSelectionGraph(SelectionGraph *g){
    selectionGraph = g;
}

void GeographicalViewWidget::setSelectionTime(QDateTime startT, QDateTime endT) {
  DateTimeList times = DateTimeList();
  times.append(DateTimePair(startT, endT));
  this->setSelectionTimes(times);
}

void GeographicalViewWidget::setSelectionTimes(const DateTimeList &list) {
  this->selectionTimes  = list;

  QString title = QString("%1 to %2")
    .arg(this->getSelectedStartTime().toString("ddd MM/dd/yy hh:mm AP"))
    .arg(this->getSelectedEndTime().toString("ddd MM/dd/yy hh:mm AP"));
  if (this->selectionTimes.count()>1)
    title = QString("%1 periods from %2").arg(this->selectionTimes.count()).arg(title);
  this->setWindowTitle(title);
  
  QWidget *viewWidget = this->parentWidget();
  while (viewWidget && (dynamic_cast<ViewWidget*>(viewWidget)==0)) {
    viewWidget = viewWidget->parentWidget();
  }
  if (viewWidget)
    viewWidget->setWindowTitle(title);
}

DateTimeList GeographicalViewWidget::getSelectionTimes()
{
  return this->selectionTimes;
}

uint GeographicalViewWidget::getSelectionDuration()
{
  uint duration = 0;
  for (int i=0; i<this->selectionTimes.count(); i++)
    duration += selectionTimes.at(i).second.toTime_t()-selectionTimes.at(i).first.toTime_t();
  return duration;
}

void GeographicalViewWidget::setSelectionType(Selection::TYPE type)
{
    selectionType = type;

    // handle nodes
    map<int,SelectionGraphNode*>::iterator nodeIteratorBegin;
    map<int,SelectionGraphNode*>::iterator nodeIteratorEnd;
    selectionGraph->getNodeIterator(nodeIteratorBegin,nodeIteratorEnd);
    bool somethingChanged = false;
    for(; nodeIteratorBegin != nodeIteratorEnd ; ++nodeIteratorBegin){
        SelectionGraphNode* node = nodeIteratorBegin->second;
        if(node->isSelected() && (node->getSelection()->getType() != type)){
            node->getSelection()->setType(type);
            somethingChanged = true;
        }
    }

    if(somethingChanged){
        querySelectedData();
        repaintContents();
    }
}

void GeographicalViewWidget::setSelectionMode(SelectionMode mode)
{
    selectionMode = mode;
}

void GeographicalViewWidget::updateData(){
    //qDebug() << "Before query selected data";
    querySelectedData();
    //qDebug() << "After query selected data";
    repaintContents();
    //qDebug() << "After repaint contents";
}

KdTrip::TripSet * GeographicalViewWidget::getSelectedTrips()
{
    return this->selectedTrips;
}

QDateTime GeographicalViewWidget::getSelectedStartTime()
{
    if(this->selectionTimes.size() == 0)
        return QDateTime(QDate(1900,1,1),QTime(0,0,0));
    return this->selectionTimes.front().first;
}

QDateTime GeographicalViewWidget::getSelectedEndTime()
{
    if(this->selectionTimes.size() == 0)
        return QDateTime(QDate(1900,1,1),QTime(0,0,0));
    return this->selectionTimes.back().second;
}

SelectionGraph *GeographicalViewWidget::getSelectionGraph()
{
    return this->selectionGraph;
}

Selection::TYPE GeographicalViewWidget::getSelectionType()
{
  return this->selectionType;
}

ColorBar *GeographicalViewWidget::getColorBar()
{
    return this->colorbar;
}

QVector2D GeographicalViewWidget::getScalarRange()
{
    QVector2D range;
    QList<RenderingLayer*> layers = this->mapView()->getRenderingLayers();
    for (int i=0; i<layers.size(); i++) {
        GridMap *grid = dynamic_cast<GridMap*>(layers[i]);
        if (grid && grid->isEnabled()) {
            QVector2D r = grid->getCellValueRange();
            range.setX(std::min(r.x(), range.x()));
            range.setY(std::max(r.y(), range.y()));
        }
    }
    return range;
}

bool GeographicalViewWidget::isSelectionTimeVisible()
{
  return this->selectionTimeVisible;
}

void GeographicalViewWidget::showSelectionTime(bool show)
{
  if (this->selectionTimeVisible!=show) {
    this->selectionTimeVisible = show;
    this->repaintContents();
  }
}

void GeographicalViewWidget::setSelectionTimeColor(QColor color)
{
  this->selectionTimeColor = color;
}

bool GeographicalViewWidget::isQueryDescriptionVisible()
{
  return this->queryDescriptionVisible;
}

void GeographicalViewWidget::showQueryDescription(bool show)
{
  if (this->queryDescriptionVisible!=show) {
    this->queryDescriptionVisible = show;
    this->repaintContents();
  }
}

void GeographicalViewWidget::setQueryDescription(const QStringList &desc)
{
  this->queryDescription = desc;
}


void GeographicalViewWidget::setAnimationEnabled(bool b)
{
    this->layerAnimation->setEnabled(b);
    this->layerLocation->setEnabled(!b);
}

void GeographicalViewWidget::loadFinished(){
    cerr << "Finished loading!" << endl;
    this->repaintContents();
}

void GeographicalViewWidget::initGL(){
    cerr << "Initializing OpenGL..." << endl;
    //generate kde texture
    //    int kdeTextureResolution = 512;
    //    GLfloat kdeTexture[kdeTextureResolution][kdeTextureResolution];
    //    for(int i = 0 ; i < kdeTextureResolution ; ++i){
    //        for(int j = 0 ; j < kdeTextureResolution ; ++j){
    //            kdeTextureResolution[i][j] = ;
    //        }
    //    }    
}

void GeographicalViewWidget::paintOverlay(QPainter *painter){
    //render selections, this should be rendered on top of the opengl stuff
    renderSelections(painter);
    //
    colorbar->paint(painter);
    //
    if (this->selectionTimeVisible)
      this->renderSelectionTime(painter);
    if (this->queryDescriptionVisible)
      this->renderQueryDescription(painter);
}

void GeographicalViewWidget::renderSelectionTime(QPainter *painter)
{
  QString dateStr = this->getSelectedStartTime().toString("ddd MM/dd/yy");
  QString timeStr = this->getSelectedStartTime().toString("h:mmAP");
  int days = this->getSelectedStartTime().secsTo(this->getSelectedEndTime().addSecs(1))/3600/24;
  if (days==0) {
    timeStr = QString("%1-%2").arg(timeStr).arg(this->getSelectedEndTime().addSecs(1).toString("h:mm AP"));
  }
  else {
    dateStr = QString("%1 days-%2").arg(days).arg(dateStr);
    timeStr = QString();
  }

  QFont font("Arial", 18);
  QFontMetrics metrics(font);
  painter->setFont(font);  
  painter->setPen(Qt::black);
  painter->drawText(QRectF(0, 1, painter->window().width(), metrics.height()), Qt::AlignRight, dateStr);
  painter->setPen(this->selectionTimeColor);
  painter->drawText(QRectF(0, 0, painter->window().width()-1, metrics.height()), Qt::AlignRight, dateStr);

  if (!timeStr.isEmpty()) {
    painter->setPen(Qt::black);
    painter->drawText(QRectF(0, metrics.height()+1, painter->window().width(), metrics.height()), Qt::AlignRight, timeStr);
    painter->setPen(this->selectionTimeColor);
    painter->drawText(QRectF(0, metrics.height(), painter->window().width()-1, metrics.height()), Qt::AlignRight, timeStr);
  }
}

void GeographicalViewWidget::renderQueryDescription(QPainter *painter)
{
  QStringList desc = this->queryDescription;
  if (desc.empty())
    desc << "No attribute constraint specified.";
  
  QFont font("Arial", 16);
  QFontMetrics metrics(font);
  painter->setFont(font);

  int margin = 20;
  int spacing = 5;
  int lineHeight = metrics.height()+spacing;
  int height = desc.count()*lineHeight-spacing;
  int topY = painter->window().height()-height-margin;
  int width = 0;

  for (int i=0; i<desc.count(); i++)
    width = std::max(width, metrics.width(desc.at(i)));
                     
  QRectF bounds(5, topY-5, width+11, height+11);
  painter->setPen(Qt::black);
  painter->setBrush(QColor(255, 255, 204));
  painter->drawRect(bounds);

  int y = topY + metrics.ascent();
  for (int i=0; i<desc.count(); i++, y+=lineHeight) {
    painter->setPen(Qt::white);
    painter->drawText(bounds.x()+6, y+1, desc.at(i));
    painter->setPen(Qt::black);
    painter->drawText(bounds.x()+5, y, desc.at(i));
  }
}

void GeographicalViewWidget::drawArrow(QPainter *painter, QLineF line)
{
    qreal arrowSize = 10;

    // Draw the line itself
    painter->drawLine(line);

    // Draw the arrows
    double angle = ::acos(line.dx() / line.length());
    if (line.dy() >= 0)
        angle = M_PI*2.0 - angle;

    QPointF destArrowP1 = line.p2() + QPointF(sin(angle - M_PI / 3) * arrowSize,
                                              cos(angle - M_PI / 3) * arrowSize);
    QPointF destArrowP2 = line.p2() + QPointF(sin(angle - M_PI + M_PI / 3) * arrowSize,
                                              cos(angle - M_PI + M_PI / 3) * arrowSize);

    painter->drawPolygon(QPolygonF() << line.p2() << destArrowP1 << destArrowP2);
}


void GeographicalViewWidget::renderSelections(QPainter *painter){

    //render selection graph
    if(selectionGraph != NULL){
        map<int,SelectionGraphNode*>::iterator nodeIteratorBegin;
        map<int,SelectionGraphNode*>::iterator nodeIteratorEnd;
        selectionGraph->getNodeIterator(nodeIteratorBegin,nodeIteratorEnd);
        QPen pen;
        pen.setWidthF(2.5);
        for(; nodeIteratorBegin != nodeIteratorEnd ; ++nodeIteratorBegin){
            SelectionGraphNode* node = nodeIteratorBegin->second;
            QColor groupColor = node->getGroup().getColor();

            QPen pen;
            pen.setWidth(3);

            if(node->isSelected())
                pen.setStyle(Qt::DashLine);

            QColor colorSel;
            if(node->inDegree() + node->outDegree() > 0){
                painter->setBrush(Qt::NoBrush);
                pen.setColor(Qt::black);
            }
            else{
                pen.setColor(groupColor);

                if (node->getSelection()->getType()==Selection::START) {
                    colorSel = Qt::blue;
                } else if (node->getSelection()->getType()==Selection::END) {
                    colorSel = Qt::red;
                } else if (node->getSelection()->getType()==Selection::START_AND_END) {
                    colorSel = Qt::green;
                }

                if(node->getSelection()->isActive()){
                    colorSel.setAlpha(20);
                } else {
                    colorSel.setAlpha(10);
                }
                painter->setBrush(QBrush(colorSel));
            }
            painter->setPen(pen);
            QPainterPath selPath = node->getSelection()->getGeometry();
            painter->drawPath(convertToScreen(selPath));
        }


        // Render the arrow for each edge
        // painter->save();
        // painter->setRenderHint(QPainter::Antialiasing,false);
        map<int,SelectionGraphEdge*>::iterator edgeIteratorBegin;
        map<int,SelectionGraphEdge*>::iterator edgeIteratorEnd;
        selectionGraph->getEdgeIterator(edgeIteratorBegin,edgeIteratorEnd);
        for(; edgeIteratorBegin != edgeIteratorEnd ; ++edgeIteratorBegin){
            SelectionGraphEdge* edge = edgeIteratorBegin->second;
            QPointF a = edge->getTail()->getSelection()->boundingBox().center();
            QPointF b = edge->getHead()->getSelection()->boundingBox().center();

            QPen pen = QPen(Qt::black, 4, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
            if(edge->isSelected())
                pen.setStyle(Qt::DashLine);

            painter->setPen(pen);
            painter->setBrush(QBrush(Qt::black));
            drawArrow(painter, QLineF(mapView()->mapFromGeoLocation(a), mapView()->mapFromGeoLocation(b)));
            painter->setPen(QPen(edge->getGroup().getColor(), 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
            painter->setBrush(QBrush(edge->getGroup().getColor()));
            drawArrow(painter, QLineF(mapView()->mapFromGeoLocation(a), mapView()->mapFromGeoLocation(b)));
        }
        // painter->restore();
    }

    //render current selection
    if(currentState == GeographicalViewWidget::RECT_SELECTION){
        //render selection
        Group nextAvailableGroup = getAvailableGroup();
        QColor color;
        if (selectionType==Selection::START) {
            color = Qt::blue;
        } else if (selectionType==Selection::END) {
            color = Qt::red;
        } else if (selectionType==Selection::START_AND_END) {
            color = Qt::green;
        }

        painter->setPen(QPen(nextAvailableGroup.getColor(), 2.5));

        //
        color.setAlphaF(0.3);
        painter->setBrush(color);
        //
        painter->drawPath(selectionPath);
    }
    else if(currentState == GeographicalViewWidget::FREE_SELECTION ||
            currentState == GeographicalViewWidget::POLYGON_SELECTION){
        //
        Group nextAvailableGroup = getAvailableGroup();
        QColor color;
        if (selectionType==Selection::START) {
            color = Qt::blue;
        } else if (selectionType==Selection::END) {
            color = Qt::red;
        } else if (selectionType==Selection::START_AND_END) {
            color = Qt::green;
        }

        painter->setPen(QPen(nextAvailableGroup.getColor(), 2.5));

        //
        color.setAlphaF(0.3);
        painter->setBrush(color);

        QPainterPath path(selectionPath);
        path.closeSubpath();
        path.setFillRule(Qt::WindingFill);

        //
        painter->drawPath(path);
    }

    //qDebug() << "Base position " << basePosition << " " << (selectionMode == LINK) << " " << (basePosition!=QPoint(-1, -1)) << endl;

    if(selectionMode == LINK && basePosition!=QPoint(-1, -1)){
        painter->setPen(QPen(Qt::black, 4, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        painter->setBrush(QBrush(Qt::black));

        //
        drawArrow(painter, linkingLine);
    }

}

void GeographicalViewWidget::pickEdge(SelectionGraphEdge*& pickedEdge, QPointF pos){
    map<int,SelectionGraphEdge*>::iterator edgeIteratorBegin;
    map<int,SelectionGraphEdge*>::iterator edgeIteratorEnd;

    //cout << "Pick Edge" << endl;

    selectionGraph->getEdgeIterator(edgeIteratorBegin,edgeIteratorEnd);
    for(; edgeIteratorBegin != edgeIteratorEnd ; ++edgeIteratorBegin){
        SelectionGraphEdge* edge = edgeIteratorBegin->second;
        //qDebug() << "   testing edge " << edge->getID();

        QPointF a = edge->getTail()->getSelection()->boundingBox().center();
        QPointF b = edge->getHead()->getSelection()->boundingBox().center();

        //qDebug() << "Edge tail center " << a << " edge head center " << b;

        QPointF screenTail = mapView()->mapFromGeoLocation(a);
        QPointF screenHead = mapView()->mapFromGeoLocation(b);

        //qDebug() << "Edge tail screen center " << screenTail << " edge head screen center " << screenHead;

        if(screenTail == screenHead)
            continue;

        QPointF v1 = pos - screenTail;
        QPointF lineVector = screenHead - screenTail;

        QPointF projOnLine = (UsefulGeometry::dotProduct(v1,lineVector)/UsefulGeometry::dotProduct(lineVector,lineVector)) * lineVector;
        QPointF distanceVector = v1 - projOnLine;
        qreal distance = UsefulGeometry::distance(distanceVector,QPointF(0,0));
        if(distance < 3){
            pickedEdge = edge;
            return;
        }
    }

    pickedEdge = NULL;
}

void GeographicalViewWidget::mousePressEvent(QMouseEvent *event){
    //cout << "GEO MOUSE PRESS EVENT" << endl;

    if (this->currentState == GeographicalViewWidget::POLYGON_SELECTION) {
        if ((event->posF()-this->basePolygon.front()).manhattanLength()<10) {
            this->basePolygon.pop_back();
            this->donePolygonSelection();
        } else {
            this->basePolygon << event->posF();
            this->selectionPath = QPainterPath();
            this->selectionPath.addPolygon(this->basePolygon);
        }
        this->repaintContents();
        return;
    }

    //pick edge
    SelectionGraphEdge* pickedEdge = NULL;
    pickEdge(pickedEdge,event->posF());

    //
    SelectionGraphNode* sel = NULL;
    pickSelection(sel,event->posF());

    if (this->currentState == GeographicalViewWidget::EDIT_SELECTION) {
        QMapWidget::mousePressEvent(event);
        QGraphicsItem *item = this->scene()->itemAt(event->posF());
        if (!(item->flags() & QGraphicsItem::ItemIsSelectable)) {
            this->doneEditingSelection();
        }
        return;
    }
    //remove edge
    else if(pickedEdge != NULL && event->modifiers() & Qt::CTRL) {
        selectionGraph->removeEdge(pickedEdge->getID());
        querySelectedData();
        currentState = GeographicalViewWidget::IDLE;
        this->repaintContents();
    }
    // remove selection
    else if(sel != NULL && event->modifiers() & Qt::CTRL) {
        selectionGraph->removeNode(sel->getId());
        querySelectedData();
        currentState = GeographicalViewWidget::IDLE;
        this->repaintContents();
    }
    else if (sel!=NULL && event->modifiers() & Qt::ALT) {
        this->tailNode = sel;
        this->basePolygon = this->tailNode->getSelection()->getGeometry().toFillPolygon();
        this->basePolygon.pop_back();
        for (unsigned i=0; i<this->basePolygon.size(); i++) {
            QGraphicsRectItem *item = this->scene()->addRect(-5, -5, 10, 10, QPen(Qt::black), QColor(160,160,160,128));
            item->setFlags(QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsSelectable);
            this->editingAnchors.push_back(item);
        }
        this->currentState = GeographicalViewWidget::EDIT_SELECTION;
        this->updateEditingAnchors();
        this->connect(this->mapView(), SIGNAL(doneUpdating()), this, SLOT(updateEditingAnchors()));
    }
    else if(event->button() == Qt::LeftButton){
        if (sel==NULL) {
            QMapWidget::mousePressEvent(event);
            currentState = GeographicalViewWidget::PANNING;
        }
        else {
            this->tailNode = sel;
            this->projectedBasePosition = this->mapView()->mapToGeoLocation(event->posF());
            this->projectedSelectionPath = this->tailNode->getSelection()->getGeometry();
            this->currentState = GeographicalViewWidget::MOVE_SELECTION;
        }
    } // Linking Nodes
    else if(event->button() == Qt::RightButton && selectionMode==LINK) {
        if (sel!=NULL) {
            tailNode = sel;
            basePosition = event->pos();
            currentState = LINKING;
        }
    }
    else if(event->button() == Qt::RightButton && selectionMode==SINGLE) {
        // Rectangle Selection
        basePosition = event->pos();
        if (event->modifiers() & Qt::SHIFT) {
            currentState = GeographicalViewWidget::RECT_SELECTION;
        }

        // Polygon Selection
        else if (event->modifiers() & Qt::ALT) {
            currentState = GeographicalViewWidget::POLYGON_SELECTION;

            //reset
            this->selectionPath = QPainterPath();

            this->basePolygon = QPolygonF();
            this->basePolygon << event->posF() << event->posF();
        }
        // Free Selection
        else {
            //reset
            selectionPath = QPainterPath();
            projectedSelectionPath = QPainterPath();

            //
            selectionPath.moveTo(event->x(),event->y());
            QPointF geoCoords = mapView()->mapToGeoLocation(event->posF());
            projectedSelectionPath.moveTo(geoCoords);
            currentState = GeographicalViewWidget::FREE_SELECTION;
        }
    } else {
        tailNode = NULL;
        basePosition = QPoint(-1,-1);
    }
}

void GeographicalViewWidget::mouseMoveEvent(QMouseEvent *event){

    if (this->currentState==GeographicalViewWidget::MOVE_SELECTION) {
        QPointF newCenter = this->mapView()->mapToGeoLocation(event->posF());
        QPainterPath newPath = this->projectedSelectionPath.translated(newCenter-this->projectedBasePosition);
        (*this->tailNode->getSelection()) = Selection(newPath, this->tailNode->getSelection()->getType());
    }
    else if (this->currentState==GeographicalViewWidget::EDIT_SELECTION) {
        QMapWidget::mouseMoveEvent(event);
        if (event->buttons()!=Qt::NoButton)
            this->updateEditingSelection();
        else
            return;
    }
    else if (selectionMode==SINGLE) {
        if(currentState == GeographicalViewWidget::RECT_SELECTION){
            int minX = std::min(basePosition.x(),event->x());
            int maxX = std::max(basePosition.x(),event->x());
            int minY = std::min(basePosition.y(),event->y());
            int maxY = std::max(basePosition.y(),event->y());

            QPainterPath path;
            path.addRect(QRectF(minX,minY,maxX-minX,maxY-minY));
            selectionPath = path;

            QPointF ll = mapView()->mapToGeoLocation(QPointF(minX,minY));
            QPointF ur = mapView()->mapToGeoLocation(QPointF(maxX,maxY));

            projectedSelectionPath = QPainterPath();
            projectedSelectionPath.addRect(QRectF(ll,ur));
        }
        else if(currentState == GeographicalViewWidget::FREE_SELECTION){
            selectionPath.lineTo(event->x(),event->y());
            QPointF geoCoords = mapView()->mapToGeoLocation(event->posF());
            projectedSelectionPath.lineTo(geoCoords);
        }
        else if(currentState == GeographicalViewWidget::POLYGON_SELECTION){
            this->basePolygon.pop_back();
            this->basePolygon << event->posF();
            this->selectionPath = QPainterPath();
            this->selectionPath.addPolygon(this->basePolygon);
        }
        else if(currentState == GeographicalViewWidget::PANNING)
            QMapWidget::mouseMoveEvent(event);

    } else if (selectionMode==LINK && basePosition!=QPoint(-1, -1)) {
        linkingLine.setP1(basePosition);
        linkingLine.setP2(event->pos());

        //qDebug() << "   mouseMove Link " << basePosition;

    } else if(selectionMode==LINK && basePosition==QPoint(-1, -1))
        QMapWidget::mouseMoveEvent(event);

    //
    this->notifyCoordinatorViewChanged();

    //
    //qDebug() << "   Conditions " << (this->currentState!=GeographicalViewWidget::IDLE) << " "<< (event->buttons()==Qt::NoButton);
    if (this->currentState!=GeographicalViewWidget::IDLE || event->buttons()==Qt::NoButton){
        //qDebug() << "   call repaint contents " << basePosition;
        this->repaintContents();
    }

    //qDebug() << "   mouseMove Final Link " << basePosition;
}

void GeographicalViewWidget::mouseReleaseEvent(QMouseEvent *event){
    if (this->currentState == GeographicalViewWidget::POLYGON_SELECTION ) {
        return;
    }
    else if (this->currentState==GeographicalViewWidget::MOVE_SELECTION) {
        querySelectedData();
    }
    else if (this->currentState==GeographicalViewWidget::EDIT_SELECTION) {
        QMapWidget::mouseReleaseEvent(event);
        querySelectedData();
        this->repaintContents();
        return;
    }
    else if(currentState == PANNING)
        QMapWidget::mouseReleaseEvent(event);
    else if(currentState == RECT_SELECTION || currentState == FREE_SELECTION){

        QRectF bb = selectionPath.boundingRect();

        if(bb.width() * bb.height() > 10){
            //
            Selection* sel = new Selection(projectedSelectionPath);
            sel->setType(selectionType);

            //
            SelectionGraphNode* node = selectionGraph->addNode(sel);
            node->setGroup(getAvailableGroup());
            querySelectedData();
        }
    } else if (selectionMode == LINK) {
        SelectionGraphNode* headNode = NULL;
        pickSelection(headNode, event->posF());

        //
        if (tailNode!=NULL && headNode != NULL) {
            Group newGroup;
            if (headNode->inDegree()!=0 || headNode->outDegree()!=0){
                newGroup = headNode->getGroup();
            }
            else{
                newGroup = tailNode->getGroup();
            }

            SelectionGraphEdge *edge=selectionGraph->addEdge(tailNode, headNode);
//            tailNode->setGroup(newGroup);
//            headNode->setGroup(newGroup);
//            tailNode->copyGroupToNeighbors();
//            headNode->copyGroupToNeighbors();
            edge->setGroup(newGroup);
            querySelectedData();
        }
    }

    //
    basePosition = QPoint(-1, -1);
    selectionPath = QPainterPath();
    projectedSelectionPath = QPainterPath();
    currentState = GeographicalViewWidget::IDLE;
    tailNode = NULL;

    this->repaintContents();
}

Group GeographicalViewWidget::getAvailableGroup(){
    assert(selectionGraph);

    set<Group> groups;

    map<int,SelectionGraphNode*>::iterator beginNodeIterator;
    map<int,SelectionGraphNode*>::iterator   endNodeIterator;
    selectionGraph->getNodeIterator(beginNodeIterator,endNodeIterator);

    for(;beginNodeIterator != endNodeIterator ; ++beginNodeIterator){
        SelectionGraphNode* node = beginNodeIterator->second;
        Group group = node->getGroup();
        groups.insert(group);
    }

    map<int,SelectionGraphEdge*>::iterator beginEdgeIterator;
    map<int,SelectionGraphEdge*>::iterator   endEdgeIterator;
    selectionGraph->getEdgeIterator(beginEdgeIterator,endEdgeIterator);

    for(;beginEdgeIterator != endEdgeIterator ; ++beginEdgeIterator){
        SelectionGraphEdge* edge = beginEdgeIterator->second;
        Group group = edge->getGroup();
        groups.insert(group);
    }

    return GroupRepository::getInstance().getNextAvailableGroup(groups);
}

QPainterPath GeographicalViewWidget::convertToScreen(QPainterPath &path){
    QPolygonF pol = path.toFillPolygon();
    QPointF* end   = pol.end();
    QPointF* it = pol.begin();


    //
    QPainterPath p;
    for(; it != end ; ++it){
        QPointF convPoint = mapView()->mapFromGeoLocation(*it);
        if (it==pol.begin())
            p.moveTo(convPoint);
        else
            p.lineTo(convPoint);
    }

    return p;
}

QPolygonF GeographicalViewWidget::convertToScreen(QPolygonF &pol){
    QPointF* end   = pol.end();
    QPointF* it = pol.begin();


    //
    QVector<QPointF> vec;
    for(; it != end ; ++it){
        QPointF convPoint = mapView()->mapFromGeoLocation(*it);
        vec.push_back(convPoint);
    }

    return QPolygonF(vec);
}

void GeographicalViewWidget::pickSelection(SelectionGraphNode *& sel, QPointF p){
    QPointF convertedPoint = mapView()->mapToGeoLocation(p);

    map<int,SelectionGraphNode*>::iterator nodeIteratorBegin;
    map<int,SelectionGraphNode*>::iterator nodeIteratorEnd;

    selectionGraph->getNodeIterator(nodeIteratorBegin,nodeIteratorEnd);
    for(; nodeIteratorBegin != nodeIteratorEnd ; ++nodeIteratorBegin){
        SelectionGraphNode* node = nodeIteratorBegin->second;
        Selection* nodeSelection = node->getSelection();
        if(nodeSelection->contains(convertedPoint)){
            sel = node;
            return;
        }
    }

    sel = NULL;
}

void GeographicalViewWidget::querySelectedData()
{
  this->selectedTrips->clear();
  KdTrip::TripSet result;
  for (int i=0; i<this->selectionTimes.count(); i++) {
    QDateTime start = this->selectionTimes.at(i).first;
    QDateTime end = this->selectionTimes.at(i).second;
    Global::getInstance()->queryData(this->selectionGraph, start, end, result);
    this->selectedTrips->insert(result.begin(), result.end());
  }
  this->setQueryDescription(QStringList());
  this->emitDatasetUpdated();
}

void GeographicalViewWidget::emitDatasetUpdated()
{
  emit datasetUpdated();
}

void GeographicalViewWidget::setCoordinator(Coordinator *c)
{
  this->coordinator = c;
}

void GeographicalViewWidget::notifyCoordinatorViewChanged(){
    if (this->coordinator && this->coordinator->containsMapView(this->mapView())) {
        QPointF center = this->mapView()->center();
        int zoomLevel = this->mapView()->zoomLevel();
        foreach(GeographicalViewWidget *widget, this->coordinator->linkedMapWidgets()) {
            if (widget->mapView()!=this->mapView()) {
                widget->mapView()->setView(center, zoomLevel);
            }
        }
    }
}

void GeographicalViewWidget::notifyCoordinatorStepBack(){
    if (this->coordinator && this->coordinator->containsMapView(this->mapView())) {
        foreach(GeographicalViewWidget *widget, this->coordinator->linkedMapWidgets()) {
            if(widget != this)
                emit widget->stepBack();
        }
    }
}

void GeographicalViewWidget::notifyCoordinatorStepForward(){
    if (this->coordinator && this->coordinator->containsMapView(this->mapView())) {
        foreach(GeographicalViewWidget *widget, this->coordinator->linkedMapWidgets()) {
            if(widget != this)
                emit widget->stepForward();
        }
    }
}

void GeographicalViewWidget::notifyCoordinatorExportSelection(){
    cout << "Export Selection" << endl;
    if (this->coordinator && this->coordinator->containsMapView(this->mapView())) {
        foreach(GeographicalViewWidget *widget, this->coordinator->linkedMapWidgets()) {
            if(widget != this)
                widget->updateSelectionGraph(this->selectionGraph);
        }
    }
}

void GeographicalViewWidget::notifyCoordinatorKeyPress(QKeyEvent *event){
  if (this->coordinator && this->coordinator->containsMapView(this->mapView())) {
    this->coordinator->setEnabled(false);
    foreach(GeographicalViewWidget *widget, this->coordinator->linkedMapWidgets()) {
      if(widget != this) {
        QKeyEvent newEvent(event->type(), event->key(), event->modifiers());
        QCoreApplication::sendEvent(widget, &newEvent);
      }
    }
    this->coordinator->setEnabled(true);
  }
}

void GeographicalViewWidget::keyPressEvent(QKeyEvent *event)
{
    int key = event->key();
    bool redirect = true;
    switch (key) {
    case Qt::Key_Space:
        this->layerAnimation->togglePlaying();
        break;

    case Qt::Key_A:
        this->setAnimationEnabled(!this->layerAnimation->isEnabled());
        this->repaintContents();
        break;

    case Qt::Key_F5:
        this->layerAnimation->reset();
        this->repaintContents();
        break;

    case Qt::Key_L:
        if (TripLocationLOD *lod = dynamic_cast<TripLocationLOD*>(this->layerLocation)) {
            lod->setLodEnabled(!lod->lodEnabled());
            if (lod->isEnabled())
              this->repaintContents();
        }
        break;

    case Qt::Key_N:
        this->layerHeatMap->setNormalized(!this->layerHeatMap->isNormalized());
        if (this->layerHeatMap->isEnabled())
            this->repaintContents();
        break;

    case Qt::Key_Escape:
        if (this->currentState==POLYGON_SELECTION)
            this->donePolygonSelection();
        if (this->currentState==EDIT_SELECTION)
            this->doneEditingSelection();
        break;
    case Qt::Key_Left:
        //cout << "Step Back" << endl;
        emit stepBack();
        notifyCoordinatorStepBack();
        redirect = false;
        break;
    case Qt::Key_Right:
        //cout << "Step Forward" << endl;
        emit stepForward();
        notifyCoordinatorStepForward();
        redirect = false;
        break;
    case Qt::Key_E:
        //cout << "Export Selection" << endl;
        notifyCoordinatorExportSelection();
        break;
    case Qt::Key_Slash:
        this->showQueryDescription(!this->queryDescriptionVisible);
        break;
    case Qt::Key_Period:
        this->showSelectionTime(!this->selectionTimeVisible);
        break;
    default:
        if (key>=Qt::Key_1 && key<=Qt::Key_9) {
            RenderingLayer *layer = this->mapView()->getRenderingLayer(key-Qt::Key_1);
            if (layer) {
                layer->setEnabled(!layer->isEnabled());
                if (dynamic_cast<TripAnimation*>(layer))
                    this->setAnimationEnabled(this->layerAnimation->isEnabled());
                if (GridMap *gm=dynamic_cast<GridMap*>(layer)) {
                    if (layer->isEnabled())
                        this->colorbar->setColorScale(gm->getColorScale());
                    else
                        this->colorbar->setColorScale(NULL);
                    this->updateColorBar();
                }
                this->repaintContents();
            }
        }
        break;
    }
    if(redirect) {
        QMapWidget::keyPressEvent(event);
        if (event->isAccepted())
          this->notifyCoordinatorViewChanged();
    }
    this->notifyCoordinatorKeyPress(event);
}

void GeographicalViewWidget::updateColorBar()
{
    QVector2D range = this->getScalarRange();
    this->colorbar->setRealMinMax(range.x(), range.y());
    if (this->coordinator && this->coordinator->containsMapView(this->mapView()))
        this->coordinator->notifyAll();
}

void GeographicalViewWidget::updateSelectionGraph(SelectionGraph *g){
    selectionGraph->assign(g);
    querySelectedData();
}

void GeographicalViewWidget::donePolygonSelection()
{
    QPainterPath path;
    path.moveTo(this->mapView()->mapToGeoLocation(this->basePolygon.front()));
    for (unsigned i=1; i<this->basePolygon.size(); i++)
        path.lineTo(this->mapView()->mapToGeoLocation(this->basePolygon.at(i)));
    // for (unsigned i=0; i<this->basePolygon.size(); i++) {
    //   QPointF p = this->mapView()->mapToGeoLocation(this->basePolygon.at(i));
    //   fprintf(stderr, "%g %g,\n", p.y(), p.x());
    // }
    Selection *sel = new Selection(path);
    sel->setType(selectionType);
    SelectionGraphNode* node = selectionGraph->addNode(sel);
    node->setGroup(getAvailableGroup());
    querySelectedData();
    basePosition = QPoint(-1, -1);
    selectionPath = QPainterPath();
    currentState = GeographicalViewWidget::IDLE;
}

void GeographicalViewWidget::doneEditingSelection()
{
    this->disconnect(this->mapView(), SIGNAL(doneUpdating()), this, SLOT(updateEditingAnchors()));
    for (unsigned i=0; i<this->editingAnchors.size(); i++)
        this->scene()->removeItem(this->editingAnchors.at(i));
    this->editingAnchors.clear();
    this->tailNode = 0;
    this->currentState = GeographicalViewWidget::IDLE;
}

void GeographicalViewWidget::updateEditingAnchors()
{
    if (this->currentState==GeographicalViewWidget::EDIT_SELECTION) {
        for (unsigned i=0; i<this->basePolygon.size(); i++) {
            this->editingAnchors.at(i)->setPos(this->mapView()->mapFromGeoLocation(this->basePolygon.at(i)));
        }
    }
}

void GeographicalViewWidget::updateEditingSelection()
{
    if (this->currentState==GeographicalViewWidget::EDIT_SELECTION) {
        this->basePolygon.clear();
        QPainterPath path;
        this->basePolygon.push_back(this->mapView()->mapToGeoLocation(this->editingAnchors.front()->pos()));
        path.moveTo(this->basePolygon.back());
        for (unsigned i=1; i<this->editingAnchors.size(); i++) {
            this->basePolygon.push_back(this->mapView()->mapToGeoLocation(this->editingAnchors.at(i)->pos()));
            path.lineTo(this->basePolygon.back());
        }
        (*this->tailNode->getSelection()) = Selection(path, this->tailNode->getSelection()->getType());
    }
}

void GeographicalViewWidget::wheelEvent(QWheelEvent *event){

    if(event->orientation() == Qt::Vertical)
        QMapWidget::wheelEvent(event);

    this->notifyCoordinatorViewChanged();

    repaintContents();
}

void GeographicalViewWidget::mouseDoubleClickEvent(QMouseEvent *event){
    if (currentState == POLYGON_SELECTION) {
        this->donePolygonSelection();
        return;
    }
    //
    SelectionGraphEdge* pickedEdge = NULL;
    pickEdge(pickedEdge,event->posF());
    if(pickedEdge != NULL){
        pickedEdge->toogleSelected();
        repaintContents();
    }
    else{
        SelectionGraphNode* sel = NULL;
        pickSelection(sel,event->posF());

        // remove selection
        if(sel != NULL){
            sel->toogleSelected();
            repaintContents();
        }
    }

    if (event->modifiers() & Qt::CTRL) {
        QList<RenderingLayer*> layers = this->mapView()->getRenderingLayers();
        for (int i=0; i<layers.size(); i++) {
          GridMap *grid = dynamic_cast<GridMap*>(layers[i]);
          if (grid && grid->isEnabled())
            grid->toggleHightlightedCellVisual();
        }
    }
    else {
        QPainterPath path;
        QList<RenderingLayer*> layers = this->mapView()->getRenderingLayers();
        for (int i=0; i<layers.size(); i++) {
            GridMap *grid = dynamic_cast<GridMap*>(layers[i]);
            if (grid && grid->isEnabled()) {
                if (GridCell *cell = grid->highlightedCell()) {
                    for (int i=0; i<cell->geometry.size(); i++)
                        path.addPolygon(cell->geometry[i]);
                    grid->setEnabled(false);
                    this->colorbar->setColorScale(NULL);
                    break;
                }
            }
        }
        if (!path.isEmpty()) {
            Selection *sel = new Selection(path);
            sel->setType(this->selectionType);
            SelectionGraphNode* node = this->selectionGraph->addNode(sel);
            node->setGroup(this->getAvailableGroup());
            this->querySelectedData();
        }
    }
}

bool GeographicalViewWidget::mergeSelections(){
    map<int,SelectionGraphNode*>::iterator beginNodeIterator;
    map<int,SelectionGraphNode*>::iterator   endNodeIterator;
    selectionGraph->getNodeIterator(beginNodeIterator,endNodeIterator);

    Group mergeGroup;
    bool  isFirst = true;
    bool  somethingChanged = false;

    for(;beginNodeIterator != endNodeIterator ; ++beginNodeIterator){
        SelectionGraphNode* node = beginNodeIterator->second;
        if(node->isSelected()){
            if(isFirst){
                isFirst = false;
                mergeGroup = node->getGroup();
            }
            else{
                node->setGroup(mergeGroup);
                somethingChanged = true;
            }
        }
    }

    map<int,SelectionGraphEdge*>::iterator beginEdgeIterator;
    map<int,SelectionGraphEdge*>::iterator   endEdgeIterator;
    selectionGraph->getEdgeIterator(beginEdgeIterator,endEdgeIterator);

    for(; beginEdgeIterator != endEdgeIterator ; ++beginEdgeIterator){
        SelectionGraphEdge* edge = beginEdgeIterator->second;
        if(edge->isSelected()){
            if(isFirst){
                isFirst = false;
                mergeGroup = edge->getGroup();
            }
            else{
                edge->setGroup(mergeGroup);
                somethingChanged = true;
            }
        }
    }


    return somethingChanged;
}

bool GeographicalViewWidget::unmergeSelections(){
    map<int,SelectionGraphNode*>::iterator beginNodeIterator;
    map<int,SelectionGraphNode*>::iterator   endNodeIterator;
    map<int,SelectionGraphNode*>::iterator   nodeIterator;
    selectionGraph->getNodeIterator(beginNodeIterator,endNodeIterator);

    bool  isFirst = true;
    bool  somethingChanged = false;
    Group group;

    //make sure all selected have the same group
    for(nodeIterator = beginNodeIterator ; nodeIterator != endNodeIterator ; ++nodeIterator){
        SelectionGraphNode* node = nodeIterator->second;
        if(node->isSelected()){
            if(isFirst){
                isFirst = false;
                group = node->getGroup();
            }
            else{
                if(!group.equals(node->getGroup()))
                    return false; //not all have the same group
            }
        }
    }

    map<int,SelectionGraphEdge*>::iterator beginEdgeIterator;
    map<int,SelectionGraphEdge*>::iterator   endEdgeIterator;
    map<int,SelectionGraphEdge*>::iterator   edgeIterator;
    selectionGraph->getEdgeIterator(beginEdgeIterator,endEdgeIterator);
    for(edgeIterator = beginEdgeIterator ; edgeIterator != endEdgeIterator ; ++edgeIterator){
        SelectionGraphEdge* edge = edgeIterator->second;
        if(edge->isSelected()){
            if(isFirst){
                isFirst = false;
                group = edge->getGroup();
            }
            else{
                if(!group.equals(edge->getGroup()))
                    return false; //not all have the same group
            }
        }
    }


    /////////////////////////////////
    isFirst = true;

    for(nodeIterator = beginNodeIterator ; nodeIterator != endNodeIterator ; ++nodeIterator){
        SelectionGraphNode* node = nodeIterator->second;
        if(node->isSelected()){
            if(isFirst){
                isFirst = false;
            }
            else{
                Group newGroup = getAvailableGroup();
                node->setGroup(newGroup);
                somethingChanged = true;
            }
        }
    }

    for(edgeIterator = beginEdgeIterator ; edgeIterator != endEdgeIterator ; ++edgeIterator){
        SelectionGraphEdge* edge = edgeIterator->second;
        if(edge->isSelected()){
            if(isFirst){
                isFirst = false;
            }
            else{
                Group newGroup = getAvailableGroup();
                edge->setGroup(newGroup);
                somethingChanged = true;
            }
        }
    }

    return somethingChanged;
}
