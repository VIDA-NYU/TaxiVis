cmake_minimum_required(VERSION 3.5)
if(POLICY_LEVEL)
    cmake_policy(VERSION 3.5)
endif()
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
    QMapTileWidget.hpp
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
    QMapTileWidget.cpp
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
set(Boost_USE_STATIC_LIBS OFF)
set(Boost_NO_BOOST_CMAKE ON)
find_package(Boost 1.42 REQUIRED COMPONENTS iostreams filesystem timer)

# Find OpenGL
find_package(OpenGL REQUIRED)

# Find GLEW
find_library(GLEW_LIBRARY NAMES GLEW glew PATHS /opt/homebrew/lib)
if(NOT GLEW_LIBRARY)
    message(FATAL_ERROR "GLEW library not found")
endif()

# Find Qt5 and related files
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Qt5 REQUIRED COMPONENTS Core Gui Widgets OpenGL Network PrintSupport)
add_definitions(-DGL_SILENCE_DEPRECATION)

# Generate Qt files (Qt5 handles most automatically with AUTOMOC/AUTOUIC/AUTORCC)
qt5_wrap_ui(UI_FILES ${FORMS})
source_group("Generated" FILES ${UI_FILES})

# Set up paths and start building
include_directories(${Boost_INCLUDE_DIR} ${OPENGL_INCLUDE_DIR} ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})
add_executable(${PROJECT_NAME} ${SOURCES} ${UI_FILES} ${RESOURCES})
target_link_libraries(${PROJECT_NAME}
    Qt5::Core
    Qt5::Gui
    Qt5::Widgets
    Qt5::OpenGL
    Qt5::Network
    Qt5::PrintSupport
    ${Boost_LIBRARIES}
    ${OPENGL_LIBRARIES}
    ${GLEW_LIBRARY})
