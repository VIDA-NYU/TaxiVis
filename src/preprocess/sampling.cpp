#include <stdio.h>
#include <stdint.h>
#include "../TaxiVis/KdTrip.hpp"
#include <boost/iostreams/device/mapped_file.hpp>
#include <QVector2D>
#include <QCursor>
#include <QGraphicsItem>
#include <QFile>

struct GridCell
{
  GridCell(): visual(NULL) {}
  QString          name;
  QList<QPolygonF> geometry;
  QRectF           boundingRect;

  int              id;
  int              strokeIndices[2];
  int              fillIndices[2];

  float            value;
  QString          label;
  QGraphicsItem   *visual;

  KdTrip::TripSet  trips;

  bool contains(const QPointF&p) const
  {
    if (!this->boundingRect.contains(p))
      return false;
    for (int i=0; i<this->geometry.size(); i++)
      if (this->geometry[i].containsPoint(p, Qt::OddEvenFill))
        return true;
    return false;
  }
};

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

int main(int argc, char **argv)
{
  uint32_t mt = KdTrip::Query::createTime(2009, 2, 2, 0, 0, 0);
  fprintf(stderr, "%u\n", mt);
  return 0;

  boost::iostreams::mapped_file fi;
  fi.open(std::string(argv[1]));
  size_t n = fi.size()/sizeof(KdTrip::Trip);
  KdTrip::Trip *trips = (KdTrip::Trip*)fi.const_data();
  // trips+=2;
  // fprintf(stderr, "%u,%u,%g,%g,%g,%g,%u,%u,%u,%u,%u,%u,%u,%u,%u\n",
  //         trips->pickup_time,
  //         trips->dropoff_time,
  //         trips->pickup_long,
  //         trips->pickup_lat,
  //         trips->dropoff_long,
  //         trips->dropoff_lat,
  //         trips->id_taxi,
  //         trips->distance,     // in 0.01 miles unit
  //         trips->fare_amount,  // in cents
  //         trips->surcharge,    // in cents
  //         trips->mta_tax,      // in cents
  //         trips->tip_amount,   // in cents
  //         trips->tolls_amount, // in cents
  //         trips->payment_type,
  //         trips->passengers
  //         );

  // return 0;
  uint32_t startTime = KdTrip::Query::createTime(2011, 1, 1, 0, 0, 0);
  uint32_t endTime   = KdTrip::Query::createTime(2012, 1, 1, 0, 0, 0);
  Grid grid;
  grid.loadFromFile("../../../data/census_tracts_geom.txt");
   
  int skip = atoi(argv[2]);
  FILE *fo = fopen(argv[3], "wb");
  fprintf(fo, "pickup_time,dropoff_time,pickup_longitude,pickup_latitude,dropoff_longitude,dropoff_latitude,taxi_id,distance,fare,surcharge,mta_tax,tips,tolls,payment_type,num_passengers,pickup_region,dropoff_region\n");
  size_t written = 0;
  for (size_t k=0; k<n; k += skip, trips += skip) {
      // fwrite(trips, sizeof(KdTrip::Trip), 1, fo);
    if (k%100000==0)
      fprintf(stderr, "\r%lu/%lu %lu", k, n, written);
    if (trips->pickup_time>=startTime && trips->pickup_time<endTime) {
      int srcId = -1, dstId = -1;
      for (int i=0; srcId==-1 && i<grid.size(); i++)
        if (grid.cells[i].contains(QPointF(trips->pickup_lat, trips->pickup_long)))
          srcId = i;
      if (srcId==-1) continue;
      for (int i=0; dstId==-1 && i<grid.size(); i++)
        if (grid.cells[i].contains(QPointF(trips->dropoff_lat, trips->dropoff_long)))
          dstId = i;
      if (dstId==-1) continue;
      written++;
      fprintf(fo, "%u,%u,%g,%g,%g,%g,%u,%u,%u,%u,%u,%u,%u,%u,%u,%d,%d\n",
              trips->pickup_time,
              trips->dropoff_time,
              trips->pickup_long,
              trips->pickup_lat,
              trips->dropoff_long,
              trips->dropoff_lat,
              trips->id_taxi,
              trips->distance,     // in 0.01 miles unit
              trips->fare_amount,  // in cents
              trips->surcharge,    // in cents
              trips->mta_tax,      // in cents
              trips->tip_amount,   // in cents
              trips->tolls_amount, // in cents
              trips->payment_type,
              trips->passengers,
              srcId,
              dstId
              );
    }
  }
  fclose(fo);
  fprintf(stderr, "\n");
}
