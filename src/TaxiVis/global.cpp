#include "global.h"
#include "CityMap.hpp"
#include <string>
#include <QFile>

Global* Global::instance = NULL;

Global::Global(){
  this->cityMap = new CityMap((std::string(DATA_DIR)+"manhattan_with_weights.txt").c_str());
  colorScale = ColorScaleFactory::getInstance(HEATED_OBJECTS);
  neighSet   = new NeighborhoodSet();
  neighSet->loadGeometry();
  //
  loadExtraFieldsData((std::string(DATA_DIR)+"extra_fields.txt").c_str());
  //
  loadDataYears((std::string(DATA_DIR)+"data_years.txt").c_str());
}

Global::~Global(){
}

void Global::loadExtraFieldsData(QString filename){
    QFile file(filename);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text)){
        qDebug() << "Could not open " << filename;
        assert(false);
    }

    QTextStream stream(&file);
    extraFields.clear();
    while(!stream.atEnd()){
        QString line = stream.readLine();
        QStringList tokens = line.split(",");
        assert(tokens.size() == 4);
        //
        QString internalName = tokens[0];
        QString screenName   = tokens[1];
        QString axisLabel    = tokens[2];
        QString activeStr    = tokens[3];
        //
        ExtraField field;
        field.internalName = internalName;
        field.screenName   = screenName;
        field.axisLabel    = axisLabel;
        if(activeStr.compare("1") == 0){
            field.active = true;
        }        
        else{
            field.active = false;
        }
        this->extraFields.push_back(field);
    }
}

void Global::loadDataYears(QString filename){
    QFile file(filename);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text)){
        qDebug() << "Could not open " << filename;
        assert(false);
    }

    this->dataYears.clear();

    QTextStream stream(&file);
    extraFields.clear();
    while(!stream.atEnd()){
        QString line = stream.readLine();
        bool ok;
        int year = line.toInt(&ok);
        assert(ok);
        this->dataYears.push_back(year);
    }
}

Global *Global::getInstance(){
    if(instance == NULL)
        instance = new Global();

    return instance;
}

void Global::queryData(SelectionGraph* queryGraph, QDateTime startTime, QDateTime endTime, KdTrip::TripSet &resultSet){
    queryManger.queryData(queryGraph,startTime,endTime,resultSet);
}

CityMap * Global::getMap() {
    return this->cityMap;
}

ColorScale *Global::getColorScale(){
    return colorScale;
}

NeighborhoodSet *Global::getNeighSet(){
    return neighSet;
}

//
int Global::numExtraFields(){
    return this->extraFields.size();
}

int Global::getIndexByScreenName(QString name){
    int index = -1;
    for(int i = 0 ; i < extraFields.size() ; ++i){
        ExtraField field = extraFields.at(i);
        if(field.screenName.compare(name) == 0){
            index = i;
            break;
        }
    }
    return index;
}

ExtraField Global::getExtraField(int index){
    return this->extraFields.at(index);
}

void Global::getDataYears(QVector<int>& result){
    result.clear();
    result = dataYears;
}
