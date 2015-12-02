#ifndef GLOBAL_H
#define GLOBAL_H

#include "querymanager.h"
#include "SelectionGraph.h"
#include "neighborhoodset.h"
#include "util/colorscale.h"

struct ExtraField{
public:
    QString internalName;
    QString screenName;
    QString axisLabel;
    bool    active;
};

class CityMap;
class Global
{
private:
    Global();
    ~Global();
    static Global* instance;

    void loadExtraFieldsData(QString filename);
    void loadDataYears(QString filename);
private:
    QueryManager      queryManger;
    CityMap*          cityMap;
    ColorScale*       colorScale;
    NeighborhoodSet*  neighSet;
    //
    QVector<ExtraField> extraFields;
    QVector<int>        dataYears;
public:
    static Global* getInstance();
  
    CityMap*         getMap();
    ColorScale*      getColorScale();
    NeighborhoodSet* getNeighSet();

    void queryData(SelectionGraph* queryGraph, QDateTime startTime, QDateTime endTime, KdTrip::TripSet &);

    //
    int        numExtraFields();
    int        getIndexByScreenName(QString name);
    ExtraField getExtraField(int index);

    //
    void       getDataYears(QVector<int>&);
};

#endif // GLOBAL_H
