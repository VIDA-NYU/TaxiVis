cmake_minimum_required(VERSION 3.16)
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

# Find Qt6 and related files
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets OpenGL OpenGLWidgets Network PrintSupport Concurrent)
add_definitions(-DGL_SILENCE_DEPRECATION)

# Generate Qt files (AUTOMOC/AUTOUIC/AUTORCC handle moc, ui and qrc)
qt_wrap_ui(UI_FILES ${FORMS})
source_group("Generated" FILES ${UI_FILES})

# Set up paths and start building
include_directories(${Boost_INCLUDE_DIR} ${OPENGL_INCLUDE_DIR} ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})
# Reuse the production widgets in offscreen plotting regression tests.
add_library(taxivis_widgets STATIC ${SOURCES} ${UI_FILES})
target_link_libraries(taxivis_widgets PUBLIC
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::OpenGL
    Qt6::OpenGLWidgets
    Qt6::Network
    Qt6::PrintSupport
    Qt6::Concurrent
    ${Boost_LIBRARIES}
    ${OPENGL_LIBRARIES}
    ${GLEW_LIBRARY})

add_executable(${PROJECT_NAME} main.cpp ${RESOURCES})
target_link_libraries(${PROJECT_NAME} PRIVATE taxivis_widgets)

if(BUILD_TESTING)
    find_package(Qt6 REQUIRED COMPONENTS Test)
    add_executable(plot_regression ../../tests/plot_regression.cpp ${RESOURCES})
    target_link_libraries(plot_regression PRIVATE taxivis_widgets Qt6::Test)
    add_executable(responsiveness_regression ../../tests/responsiveness_regression.cpp ${RESOURCES})
    target_link_libraries(responsiveness_regression PRIVATE taxivis_widgets Qt6::Test)
    add_test(NAME responsiveness_regression COMMAND responsiveness_regression)
    set_tests_properties(responsiveness_regression PROPERTIES TIMEOUT 180 ENVIRONMENT
        "QT_QPA_PLATFORM=offscreen;TAXIVIS_DATA=${CMAKE_CURRENT_SOURCE_DIR}/../../data/sample_merged_1.kdtrip;TZ=America/New_York")
    # Measurement aid for the responsiveness scope decision; not a CTest case.
    add_executable(interaction_bench ../../tests/interaction_bench.cpp ${RESOURCES})
    target_link_libraries(interaction_bench PRIVATE taxivis_widgets Qt6::Test)
    # Drives the real window with a live GL viewport; needs a display, so not a CTest case.
    add_executable(native_interaction_check ../../tests/native_interaction_check.cpp ${RESOURCES})
    target_link_libraries(native_interaction_check PRIVATE taxivis_widgets Qt6::Test)
    # Regenerates doc/figs from the running application; same requirements.
    add_executable(make_figures ../../tests/make_figures.cpp ${RESOURCES})
    target_link_libraries(make_figures PRIVATE taxivis_widgets Qt6::Test)
    add_test(NAME plot_regression COMMAND plot_regression)
    set_tests_properties(plot_regression PROPERTIES ENVIRONMENT
        "QT_QPA_PLATFORM=offscreen;TAXIVIS_DATA=${CMAKE_CURRENT_SOURCE_DIR}/../../data/sample_merged_1.kdtrip;TZ=America/New_York")
endif()
