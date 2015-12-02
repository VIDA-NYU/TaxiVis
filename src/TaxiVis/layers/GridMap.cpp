#include "GridMap.hpp"
#include "QMapView.hpp"
#include "Triangulator.hpp"
#include "global.h"
#include "geographicalviewwidget.h"
#include "temporalseriesplotwidget.h"
#include "util/colorbar.h"
#include <QtGui/QVector2D>
#include <QtGui/QCursor>
#include <QtGui/QGraphicsItem>
#include <QtOpenGL/QGLShaderProgram>

struct Grid {
  size_t size() {
    return this->cells.size();
  }

  void clear() {
    this->idMap.clear();
    this->cells.clear();
  }

  void addCell(GridCell &cell)
  {
    cell.id = this->cells.size();;
    this->idMap[cell.name] = cell.id;
    this->cells.push_back(cell);
  }

  void loadFromFile(QString filename)
  {
    QFile f(filename);
    f.open(QIODevice::ReadOnly | QIODevice::Text);
    this->clear();
    int N = f.readLine().trimmed().toInt();
    for (int i=0; i<N; i++) {
      GridCell cell;
      cell.name = f.readLine().trimmed();
      cell.boundingRect = QRectF();
      cell.value = (float)i/N;
      int nPolys = f.readLine().trimmed().toInt();
      for (int j=0; j<nPolys; j++) {
        int nPoints = f.readLine().trimmed().toInt();
        QPolygonF geom;
        for (int k=0; k<nPoints; k++) {
          double lat, lon;
          QByteArray line = f.readLine();
          sscanf(line.constData(), "%lf %lf\n", &lon, &lat);
          geom << QPointF(lat, lon);
        }
        cell.geometry << geom;
        cell.boundingRect = cell.boundingRect.united(geom.boundingRect());
      }
      this->addCell(cell);
    }
    f.close();
  }

  typedef QHash<QString, int> CellIdMap;
  CellIdMap             idMap;
  std::vector<GridCell> cells;
};

GridMap::GridMap(GeographicalViewWidget *gw) :
    RenderingLayer(false),
    dataReady(false),
    bufferDirty(false),
    visualDirty(false),
    labelFont("Arial", 24),
    labelMetrics(labelFont),
    colorBorder(128, 128, 128, 128),
    colorFill(255, 0, 0, 255),
    geoWidget(gw)
{
  this->colorScale = ColorScaleFactory::getInstance(SEQUENTIAL_SINGLE_HUE_RED);
  this->grid = new Grid();
  this->connect(this->geoWidget->mapView(), SIGNAL(viewChanged(QPointF,int)),
                this, SLOT(viewChanged(QPointF,int)));
  this->connect(this->geoWidget->mapView(), SIGNAL(doneUpdating()),
                this, SLOT(doneUpdating()));
}

GridMap::~GridMap()
{
  delete this->grid;
}

void GridMap::loadGrid(QString gridFile)
{
  this->grid->loadFromFile(gridFile);
  this->buildCells();
  if (this->visualDirty)
    this->computeVisualData();
}

void GridMap::setEnabled(bool r)
{
  if (this->enabled!=r) {
    this->enabled = r;
    if (this->enabled && this->dataReady && this->visualDirty)
        this->computeVisualData();
    if (!this->enabled)
      this->highlightId = -1;
  }
}

QColor GridMap::borderColor()
{
  return this->colorBorder;
}

void GridMap::setBorderColor(QColor color)
{
  this->colorBorder = color;
}

QColor GridMap::fillColor()
{
  return this->colorFill;
}

void GridMap::setFillColor(QColor color)
{
  this->colorFill = color;
}

void GridMap::setColorScale(ColorScale *cs)
{
  this->colorScale = cs;
}

ColorScale *GridMap::getColorScale()
{
  return this->colorScale;
}

QVector2D GridMap::getCellValueRange()
{
  return this->cellValueRange;
}

void GridMap::render(QPainter *painter)
{
  if (this->enabled && this->dataReady) {
    this->useGLPainting(painter);
    this->renderGL();
    if (!this->visualDirty)
      this->renderLabel(painter);
  }
}

void GridMap::updateData()
{
  this->visualDirty = true;
  if (this->enabled && this->dataReady)
    this->computeVisualData();
}

void GridMap::computeVisualData()
{
  this->cellValueRange = QVector2D();
  this->aggregateBegin();
  for (int i=0; i<this->grid->size(); i++)
    this->grid->cells[i].trips.clear();
  KdTrip::TripSet::iterator it;
  KdTrip::TripSet *selectedTrips = this->geoWidget->getSelectedTrips();
  Selection::TYPE stype = this->geoWidget->getSelectionType();
  bool usePickup = stype==Selection::START || stype==Selection::START_AND_END;
  bool useDropoff = stype==Selection::END || stype==Selection::START_AND_END;
  for (it=selectedTrips->begin(); it!=selectedTrips->end(); it++) {
    const KdTrip::Trip *trip = *it;
    for (int i=0; i<this->grid->size(); i++) {
      bool ok = true;
      if (usePickup)
        ok = ok && this->grid->cells[i].contains(QPointF(trip->pickup_lat, trip->pickup_long));
      if (useDropoff)
        ok = ok && this->grid->cells[i].contains(QPointF(trip->dropoff_lat, trip->dropoff_long));
      if (ok) {
        this->aggregateUpdate(this->grid->cells[i].id, trip);
        this->grid->cells[i].trips.insert(trip);
        break;
      }
    }

    // int srcId = -1, dstId = -1;
    // for (int i=0; srcId==-1 && i<this->grid->size(); i++)
    //   if (this->grid->cells[i].contains(QPointF(trip->pickup_lat, trip->pickup_long)))
    //     srcId = i;
    // for (int i=0; dstId==-1 && i<this->grid->size(); i++)
    //   if (this->grid->cells[i].contains(QPointF(trip->dropoff_lat, trip->dropoff_long)))
    //     dstId = i;
    // if (srcId!=-1 && dstId!=-1) {
    //   fprintf(stdout, "%u,%u,%g,%g,%g,%g,%u,%u,%u,%u,%u,%u,%u,%u,%u,%d,%d\n",
    //           trip->pickup_time,
    //           trip->dropoff_time,
    //           trip->pickup_long,
    //           trip->pickup_lat,
    //           trip->dropoff_long,
    //           trip->dropoff_lat,
    //           trip->id_taxi,
    //           trip->distance,     // in 0.01 miles unit
    //           trip->fare_amount,  // in cents
    //           trip->surcharge,    // in cents
    //           trip->mta_tax,      // in cents
    //           trip->tip_amount,   // in cents
    //           trip->tolls_amount, // in cents
    //           trip->payment_type,
    //           trip->passengers,
    //           srcId,
    //           dstId
    //           );
    // }
  }
  this->aggregateEnd();
  for (int i=0; i<this->grid->size(); i++)
    this->aggregateOutput(this->grid->cells[i]);
  for (CellSet::const_iterator it=this->visualCells.begin(); it!=this->visualCells.end(); it++)
    this->updateCellVisualContents(*it);
  this->visualDirty = false;
  
  this->geoWidget->updateColorBar();  
}

void GridMap::initGL()
{
  glewInit();
  this->buffer[0].generate();
  this->buffer[1].generate();
  this->buffer[2].generate();
  const QGLContext *context = QGLContext::currentContext();
  if (context) {
    this->shader = PQGLShaderProgram(new QGLShaderProgram(context));
    this->shader->addShaderFromSourceFile(QGLShader::Vertex, ":/Resources/shaders/geo2screen.120.vert");
    this->shader->link();
    this->fbo = PQGLFramebufferObject(new QGLFramebufferObject(1024, 1024));
  }
}

void GridMap::buildCells()
{
  this->vertices.clear();
  this->indices.clear();
  this->triangles.clear();

  Triangulator::Vector2dVector contour, tris;
  for (unsigned k=0; k<this->grid->size(); k++) {
    GridCell &cell = this->grid->cells[k];
    cell.fillIndices[0] = this->triangles.size()/2;
    cell.strokeIndices[0] = this->indices.size();
    for (int i=0; i<cell.geometry.size(); i++) {
      QPolygonF poly = cell.geometry.at(i);
      contour.clear();
      for (int j=0; j<poly.size(); j++) {
        QPointF vert = poly.at(j);
        if (j<poly.size()-1) {
          this->indices.push_back(this->vertices.size()/2);
          this->indices.push_back(this->vertices.size()/2+1);
          contour.push_back(Triangulator::Vector2d(vert.x(), vert.y()));
        }
        this->vertices.push_back(vert.x());
        this->vertices.push_back(vert.y());

      }
      tris.clear();
      Triangulator::Triangulate::Process(contour, tris);
      for (size_t i=0; i<tris.size(); i++) {
        this->triangles.push_back(tris[i].GetX());
        this->triangles.push_back(tris[i].GetY());
      }
    }
    cell.fillIndices[1] = this->triangles.size()/2-cell.fillIndices[0];
    cell.strokeIndices[1] = this->indices.size()-cell.strokeIndices[0];
  }

  this->bufferDirty = true;
  this->dataReady = true;
}

void GridMap::renderPicking()
{
  this->highlightId = -1;
  QSizeF size = this->geoWidget->mapView()->size();
  QPoint p = this->geoWidget->mapFromGlobal(QCursor::pos());
  p.setX(p.x()*(this->fbo->size().width()-1)/(size.width()-1));
  p.setY((size.height()-1-p.y())*(this->fbo->size().height()-1)/(size.height()-1));
  if (p.x()<0 || p.x()>=this->fbo->size().width() ||
      p.y()<0 || p.y()>=this->fbo->size().height())
    return;

  glPushAttrib(GL_ALL_ATTRIB_BITS);
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glViewport(0, 0, this->fbo->size().width(), this->fbo->size().height());
  glLoadIdentity();
  glOrtho(0, size.width(), size.height(), 0, -99, 99);
  this->fbo->bind();
  glClearColor(1,1,1,1);
  glClear(GL_COLOR_BUFFER_BIT);
  glBindBuffer(GL_ARRAY_BUFFER, this->buffer[2].buffer);
  glVertexPointer(2, GL_FLOAT, 0, 0);
  for (unsigned i=0; i<this->grid->cells.size(); i++) {
    GridCell &cell = this->grid->cells[i];
    glColor4ubv((GLubyte*)(&cell.id));
    glDrawArrays(GL_TRIANGLES, cell.fillIndices[0], cell.fillIndices[1]);
  }
  glReadPixels(p.x(), p.y(), 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, &this->highlightId);
  this->fbo->release();
  glPopMatrix();
  glPopAttrib();
}

void GridMap::renderGL()
{
  if (this->bufferDirty) {
    this->buffer[0].setData(GL_ARRAY_BUFFER, this->vertices.size()*sizeof(float), &this->vertices[0], GL_DYNAMIC_DRAW);
    this->buffer[1].setData(GL_ELEMENT_ARRAY_BUFFER, this->indices.size()*sizeof(GLuint), &this->indices[0], GL_DYNAMIC_DRAW);
    this->buffer[2].setData(GL_ARRAY_BUFFER, this->triangles.size()*sizeof(float), &this->triangles[0], GL_DYNAMIC_DRAW);
    this->bufferDirty = false;
  }

  this->shader->bind();
  QSizeF size = this->geoWidget->mapView()->size();
  this->shader->setUniformValue("zoom", (float)this->geoWidget->mapView()->zoomLevel());
  this->shader->setUniformValue("center", this->geoWidget->mapView()->center());
  this->shader->setUniformValue("size", QVector2D(size.width(), size.height()));

  glEnableClientState(GL_VERTEX_ARRAY);
  this->renderPicking();

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnableClientState(GL_VERTEX_ARRAY);

  glBindBuffer(GL_ARRAY_BUFFER, this->buffer[2].buffer);
  glVertexPointer(2, GL_FLOAT, 0, 0);

  qreal range, shift;
  if (ColorBar *cb = this->geoWidget->getColorBar()) {
    range = cb->getRealMax()-cb->getRealMin();
    shift = cb->getRealMin();
  }
  else {
    range = this->cellValueRange.y()-this->cellValueRange.x();
    shift = this->cellValueRange.x();
  }
  if (range<1e-6) range = 1.0;
  for (unsigned i=0; i<this->grid->cells.size(); i++) {
    GridCell &cell = this->grid->cells[i];
    QColor color = this->colorScale->getColor((cell.value-shift)/range);
    glColor4f(color.redF(), color.greenF(), color.blueF(), 0.7);
    glDrawArrays(GL_TRIANGLES, cell.fillIndices[0], cell.fillIndices[1]);
  }

  if (this->highlightId>=0) {
    GridCell &cell = this->grid->cells[this->highlightId];
    glColor4f(1, 0, 0, 0.2);
    glDrawArrays(GL_TRIANGLES, cell.fillIndices[0], cell.fillIndices[1]);
  }

  glColor4f(this->colorBorder.redF(),
            this->colorBorder.greenF(),
            this->colorBorder.blueF(),
            this->colorBorder.alphaF());
  glBindBuffer(GL_ARRAY_BUFFER, this->buffer[0].buffer);
  glVertexPointer(2, GL_FLOAT, 0, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->buffer[1].buffer);
  glDrawElements(GL_LINES, this->indices.size(), GL_UNSIGNED_INT, 0);

  if (this->highlightId>=0) {
    GridCell &cell = this->grid->cells[this->highlightId];
    glColor4f(this->colorFill.redF(),
              this->colorFill.greenF(),
              this->colorFill.blueF(),
              this->colorFill.alphaF());
    glLineWidth(2.0f);
    glDrawElements(GL_LINES, cell.strokeIndices[1], GL_UNSIGNED_INT, (void*)(cell.strokeIndices[0]*sizeof(GLuint)));
    glLineWidth(1.0f);
  }

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glDisableClientState(GL_VERTEX_ARRAY);
  glDisable(GL_BLEND);

  this->shader->release();
}

void GridMap::renderLabel(QPainter *painter)
{
  if (this->highlightId>=0) {
    GridCell &cell = this->grid->cells[this->highlightId];
    this->useQtPainting(painter);
    QPainterPath path;
    path.addText(-this->labelMetrics.width(cell.label)*0.5,
                 this->labelMetrics.ascent()*0.5,
                 this->labelFont, cell.label);
    path.translate(this->geoWidget->mapView()->mapFromGeoLocation(cell.boundingRect.center()));
    painter->setPen(Qt::black);
    painter->setBrush(Qt::black);
    painter->drawPath(path);
  }
}

GridCell *GridMap::highlightedCell()
{
  if (this->highlightId<0) return NULL;
  return &(this->grid->cells[this->highlightId]);
}

void GridMap::toggleHightlightedCellVisual()
{
  GridCell *cell = this->highlightedCell();
  if (cell) {
    if (!cell->visual) {
      cell->visual = this->createCellVisual(cell);
      if (cell->visual) {
        this->updateCellVisualGeometry(cell);
        this->geoWidget->mapView()->scene()->addItem(cell->visual);
        this->visualCells.insert(cell);
      }
    }
    else {
      this->geoWidget->mapView()->scene()->removeItem(cell->visual);
      this->visualCells.erase(cell);
      delete cell->visual;
      cell->visual = NULL;
    }
  }
}

void GridMap::updateCellVisualGeometry(GridCell *cell)
{
  QRectF br = cell->visual->boundingRect();
  cell->visual->setTransformOriginPoint(QPointF(br.width()*0.5, br.height()*0.5));
  cell->visual->setScale(exp2(std::min(this->geoWidget->mapView()->zoomLevel()-17, 0)));
  QPointF p = this->geoWidget->mapView()->mapFromGeoLocation(cell->boundingRect.center());
  p -= QPointF(br.width()*0.5, br.height()*0.5);
  cell->visual->setPos(p);
}

QGraphicsItem *GridMap::createCellVisual(GridCell *cell)
{
  TemporalSeriesPlotWidget *widget = new TemporalSeriesPlotWidget();
  widget->setDateTimes(this->geoWidget->getSelectedStartTime(),
                       this->geoWidget->getSelectedEndTime());
  widget->setSelectionGraph(this->geoWidget->getSelectionGraph());
  widget->setSelectedTripsRepository(&cell->trips);
  widget->setNumberOfBins(100);
  QGraphicsProxyWidget *proxy = new QGraphicsProxyWidget();
  proxy->setWidget(widget);
  return proxy;
}

void GridMap::updateCellVisualContents(GridCell *cell)
{
  QGraphicsProxyWidget *proxy = dynamic_cast<QGraphicsProxyWidget*>(cell->visual);
  TemporalSeriesPlotWidget *widget = dynamic_cast<TemporalSeriesPlotWidget*>(proxy->widget());
  widget->setDateTimes(this->geoWidget->getSelectedStartTime(),
                       this->geoWidget->getSelectedEndTime());
  widget->setSelectionGraph(this->geoWidget->getSelectionGraph());
  widget->setSelectedTripsRepository(&cell->trips);
  widget->recomputePlots();
}

void GridMap::viewChanged(QPointF, int)
{
  for (CellSet::const_iterator it=this->visualCells.begin(); it!=this->visualCells.end(); it++)
    this->updateCellVisualGeometry(*it);
}

void GridMap::doneUpdating()
{
  for (CellSet::const_iterator it=this->visualCells.begin(); it!=this->visualCells.end(); it++)
    this->updateCellVisualGeometry(*it);
}

NumTripsGridMap::NumTripsGridMap(GeographicalViewWidget *gw)
    : GridMap(gw)
{
}

void NumTripsGridMap::aggregateBegin()
{
  this->counts.clear();
  this->counts.resize(this->grid->size(), 0);
}

void NumTripsGridMap::aggregateUpdate(int id, const KdTrip::Trip *)
{
  this->counts[id]++;
}

void NumTripsGridMap::aggregateEnd()
{
  this->cellValueRange = QVector2D(std::numeric_limits<qreal>::max(), 0);
  for (unsigned i=0; i<this->counts.size(); i++) {
    if (this->counts[i]<this->cellValueRange.x())
      this->cellValueRange.setX(this->counts[i]);
    if (this->counts[i]>this->cellValueRange.y())
      this->cellValueRange.setY(this->counts[i]);
  }
}

void NumTripsGridMap::aggregateOutput(GridCell &cell)
{
  cell.value = (float)this->counts[cell.id];
  cell.label = QString("%1 (%2)").arg(cell.name).arg(this->counts[cell.id]);
}

FarePerMileGridMap::FarePerMileGridMap(GeographicalViewWidget *gw)
    : GridMap(gw)
{
}

void FarePerMileGridMap::aggregateBegin()
{
  this->counts.clear();
  this->counts.resize(this->grid->size(), 0);
  this->fares.clear();
  this->fares.resize(this->grid->size(), 0.f);
}

void FarePerMileGridMap::aggregateUpdate(int id, const KdTrip::Trip *trip)
{
  if (trip->distance<0.25) return;
  this->counts[id]++;
  this->fares[id] += (float)trip->fare_amount/trip->distance;
}

void FarePerMileGridMap::aggregateEnd()
{
  this->cellValueRange = QVector2D(std::numeric_limits<qreal>::max(), 0);
  for (unsigned i=0; i<this->counts.size(); i++) {
    if (this->counts[i]==0) continue;
    this->fares[i] /= this->counts[i];
    if (this->fares[i]<this->cellValueRange.x())
      this->cellValueRange.setX(this->fares[i]);
    if (this->fares[i]>this->cellValueRange.y())
      this->cellValueRange.setY(this->fares[i]);
  }
}

void FarePerMileGridMap::aggregateOutput(GridCell &cell)
{
  cell.value = this->fares[cell.id];
  cell.label = QString("%1 ($%2/mile)").arg(cell.name).arg(this->fares[cell.id], 0, 'g', 2);
}

PickupDropoffGridMap::PickupDropoffGridMap(GeographicalViewWidget *gw)
    : GridMap(gw)
{
  this->colorFill = QColor(0, 0, 0, 0);
}

void PickupDropoffGridMap::aggregateBegin()
{
  this->counts.clear();
  this->counts.resize(this->grid->size(), 0);
  this->ratios.clear();
  this->ratios.resize(this->grid->size(), std::vector<float>(this->grid->size(), 0.f));
}

void PickupDropoffGridMap::aggregateUpdate(int srcId, const KdTrip::Trip *trip)
{
  int dstId = -1;
  for (int i=0; i<this->grid->size(); i++) {
    if (this->grid->cells[i].contains(QPointF(trip->dropoff_lat, trip->dropoff_long))) {      
      dstId = i;
      break;
    }
  }
  if (dstId!=-1) {
    this->counts[srcId]++;
    this->ratios[srcId][dstId] += 1.f;
  }
}

void PickupDropoffGridMap::aggregateEnd()
{
  this->cellValueRange = QVector2D(0.0, 1.0);
  for (unsigned srcId=0; srcId<this->ratios.size(); srcId++) {
    if (this->counts[srcId]>0) 
      for (unsigned dstId=0; dstId<this->ratios[srcId].size(); dstId++)
        this->ratios[srcId][dstId] /= this->counts[srcId];
  }
}

void PickupDropoffGridMap::aggregateOutput(GridCell &cell)
{
  cell.value = 0.0;
  cell.label = QString("%1 (%2)").arg(cell.name).arg(this->counts[cell.id]);
}

void PickupDropoffGridMap::renderPicking()
{
  int srcId = this->highlightId;
  GridMap::renderPicking();
  if (this->highlightId!=srcId) {
    srcId = this->highlightId;
    for (unsigned dstId=0; dstId<this->grid->cells.size(); dstId++) {
      GridCell &cell = this->grid->cells[dstId];
      if (srcId>=0)
        cell.value = this->ratios[srcId][dstId];
      else
        cell.value = 0.f;
    }
  }
}
