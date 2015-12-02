cmake_minimum_required(VERSION 2.6)
set(CMAKE_BUILD_TYPE Release CACHE STRING "Set build type.")
project(TaxiVis)

# General definitions
add_definitions(-D_FILE_OFSET_BITS=64)
add_definitions(-DRESOURCES_DIR=\"${CMAKE_CURRENT_SOURCE_DIR}/Resources/\")
add_definitions(-DDATA_DIR=\"${CMAKE_CURRENT_SOURCE_DIR}/../../data/\")

# Project files
set(QT_HEADERS 
    HistogramDialog.hpp
    TemporalSeriesDialog.hpp
    TimeExplorationDialog.hpp
    QMapView.hpp
    QMapWidget.hpp
    coordinator.h
    extendedhistogram.h
    extendedplotwidget.h
    geographicalviewwidget.h
    histogramwidget.h
    layers/GridMap.hpp
    layers/HeatMap.hpp
    layers/TripAnimation.hpp
    layers/TripLocation.hpp
    layers/TripLocationLOD.hpp
    mainwindow.h
    qcustomplot.h
    scatterplotwidget.h
    temporalseriesplotwidget.h
    timeselectionwidget.h
    util/colorbar.h
    viewwidget.h
    timewidget.h
)

set(FORMS
    HistogramDialog.ui
    TemporalSeriesDialog.ui
    TimeExplorationDialog.ui
    histogramwidget.ui
    layers/TripAnimationConfig.ui
    layers/TripAnimationToolBar.ui
    mainwindow.ui
    scatterplotwidget.ui
    temporalseriesplotwidget.ui
    timeselectionwidget.ui
    viewwidget.ui
    timewidget.ui
)

set(SOURCES
    HistogramDialog.cpp
    TemporalSeriesDialog.cpp
    TimeExplorationDialog.cpp
    Group.cpp
    GroupRepository.cpp
    QMapView.cpp
    QMapWidget.cpp
    RenderingLayer.cpp
    Selection.cpp
    SelectionGraph.cpp
    coordinator.cpp
    extendedhistogram.cpp
    extendedplotwidget.cpp
    geographicalviewwidget.cpp
    global.cpp
    histogramwidget.cpp
    layers/GridMap.cpp
    layers/HeatMap.cpp
    layers/Triangulator.cpp
    layers/TripAnimation.cpp
    layers/TripLocation.cpp
    layers/TripLocationLOD.cpp
    main.cpp
    mainwindow.cpp
    neighborhood.cpp
    neighborhoodgraph.cpp
    neighborhoodset.cpp
    qcustomplot.cpp
    querymanager.cpp
    scatterplotwidget.cpp
    temporalseriesplotwidget.cpp
    timeselectionwidget.cpp
    util/bluetocyanscale.cpp
    util/bluetoyellowscale.cpp
    util/colorbar.cpp
    util/colorscale.cpp
    util/divergent.cpp
    util/grayscale.cpp
    util/greentowhitescale.cpp
    util/heatedobjectscale.cpp
    util/lineargrayscale.cpp
    util/locsscale.cpp
    util/pseudorainbowscale.cpp
    util/rainbowscale.cpp
    util/scalar.cpp
    util/sequentialred.cpp
    util/yellowtobrownscale.cpp
    viewwidget.cpp
    timewidget.cpp
)
set(RESOURCES resources.qrc)


# Find Boost
set(Boost_USE_STATIC_LIBS ON)
find_package(Boost 1.42 COMPONENTS system iostreams filesystem timer REQUIRED)

# Find OpenGL
find_package(OpenGL REQUIRED)

# Find Qt and related files
set(QT_USE_QTWEBKIT TRUE)
set(QT_USE_QTOPENGL TRUE)
find_package(Qt4 REQUIRED)
include(${QT_USE_FILE})
add_definitions(${QT_DEFINITIONS})

# Generate Qt files
Qt4_wrap_ui(UI_FILES ${FORMS})
Qt4_wrap_cpp(MOC_FILES ${QT_HEADERS} ${UI_FILES})
Qt4_add_resources(RCC_FILES ${RESOURCES})
source_group("Generated" FILES ${UI_FILES} ${MOC_FILES} ${RCC_FILES})

# Set up paths and start building
include_directories(${Boost_INCLUDE_DIR} ${OPENGL_INCLUDE_DIR} ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})
add_executable(${PROJECT_NAME} ${SOURCES} ${MOC_FILES} ${RCC_FILES})
target_link_libraries(${PROJECT_NAME} ${QT_LIBRARIES} ${Boost_LIBRARIES} ${OPENGL_LIBRARIES} GLEW)
