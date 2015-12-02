#-------------------------------------------------
#
# Project created by QtCreator 2013-02-13T18:44:40
#
#-------------------------------------------------

QT       += core gui opengl webkit

#CONFIG += Debug

TARGET = TaxiVis
TEMPLATE = app

DEFINES += RESOURCES_DIR=\\\"$$PWD/Resources/\\\"
DEFINES += DATA_DIR=\\\"$$PWD/../../data/\\\"

SOURCES += main.cpp\
        mainwindow.cpp \
    HistogramDialog.cpp \
    TemporalSeriesDialog.cpp \
    TimeExplorationDialog.cpp \
    QMapView.cpp \
    QMapWidget.cpp \
    geographicalviewwidget.cpp \
    global.cpp \
    qcustomplot.cpp \
    SelectionGraph.cpp \
    Selection.cpp \
    GroupRepository.cpp \
    Group.cpp \
    timeselectionwidget.cpp \
    timewidget.cpp \
    viewwidget.cpp \
    coordinator.cpp \
    histogramwidget.cpp \
    temporalseriesplotwidget.cpp \
    util/divergent.cpp \
    util/colorscale.cpp \
    util/yellowtobrownscale.cpp \
    util/rainbowscale.cpp \
    util/pseudorainbowscale.cpp \
    util/locsscale.cpp \
    util/lineargrayscale.cpp \
    util/heatedobjectscale.cpp \
    util/greentowhitescale.cpp \
    util/grayscale.cpp \
    util/bluetoyellowscale.cpp \
    util/bluetocyanscale.cpp \
    util/colorbar.cpp \
    neighborhoodset.cpp \
    neighborhood.cpp \
    util/scalar.cpp \
    neighborhoodgraph.cpp \
    extendedplotwidget.cpp \
    RenderingLayer.cpp \
    layers/GridMap.cpp \
    layers/HeatMap.cpp \
    layers/Triangulator.cpp \
    layers/TripAnimation.cpp \
    layers/TripLocation.cpp \
    layers/TripLocationLOD.cpp \
    scatterplotwidget.cpp \
    util/sequentialred.cpp \
    extendedhistogram.cpp \
    querymanager.cpp

HEADERS  += mainwindow.h \
    HistogramDialog.hpp \
    TemporalSeriesDialog.hpp \
    TimeExplorationDialog.hpp \
    QMapView.hpp \
    QMapWidget.hpp \
    layers/GridMap.hpp \
    layers/HeatMap.hpp \
    layers/TripAnimation.hpp \
    layers/TripLocation.hpp \
    layers/TripLocationLOD.hpp \
    geographicalviewwidget.h \
    KdTrip.hpp \
    global.h \
    qcustomplot.h \
    SelectionGraph.h \
    Selection.h \
    UsefulGeometryFunctions.h \
    GroupRepository.h \
    Group.h \
    timeselectionwidget.h \
    timewidget.h \
    viewwidget.h \
    coordinator.h \
    histogramwidget.h \
    temporalseriesplotwidget.h \
    util/divergent.h \
    util/colorscale.h \
    util/yellowtobrownscale.h \
    util/rainbowscale.h \
    util/pseudorainbowscale.h \
    util/locsscale.h \
    util/lineargrayscale.h \
    util/heatedobjectscale.h \
    util/greentowhitescale.h \
    util/grayscale.h \
    util/bluetoyellowscale.h \
    util/bluetocyanscale.h \
    util/colorbar.h \
    neighborhoodset.h \
    neighborhood.h \
    util/scalar.h \
    neighborhoodgraph.h \
    extendedplotwidget.h \
    scatterplotwidget.h \
    util/sequentialred.h \
    extendedhistogram.h \
    querymanager.h

FORMS    += mainwindow.ui \
    timeselectionwidget.ui \
    timewidget.ui \
    viewwidget.ui \
    histogramwidget.ui \
    temporalseriesplotwidget.ui \
    layers/TripAnimationConfig.ui \
    layers/TripAnimationToolBar.ui \
    scatterplotwidget.ui \
    HistogramDialog.ui \
    TemporalSeriesDialog.ui \
    TimeExplorationDialog.ui

LIBS += -L/usr/local/lib -lGLEW
LIBS += -L/usr/local/lib -lboost_iostreams -lboost_system -lboost_filesystem

RESOURCES += \
    resources.qrc
