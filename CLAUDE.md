# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

TaxiVis (also referred to as TLCVis) is a Qt4-based visual analytics application for exploring NYC taxi trip data. The application provides interactive visualization of taxi trips using OpenGL rendering, with support for spatial and temporal queries.

## Build System

### Building the Main Application

The project uses CMake (minimum 2.6) as its primary build system:

```bash
cd src/TaxiVis
mkdir build
cd build
cmake ../
make
```

### Building Preprocessing Tools

Data preprocessing utilities are built separately:

```bash
cd src/preprocess
mkdir build
cd build
cmake ../
make
```

This generates executables: `csv2Binary`, `newFormatCsv2Binary`, `multiCsv2Binary`, `build_kdtrip`, `sampling`, `testQuery`, `unif96_to_bin`.

### Dependencies

- CMake (2.6+)
- Qt 4 (4.8.5+) with QtWebkit and QtOpenGL
- OpenGL/GLEW (1.10.0-3+)
- Boost (1.42+): system, iostreams, filesystem, timer components

## Data Management

### Data Location

- Default data directory: `data/` (relative to repository root)
- Configured via `DATA_DIR` macro in [CMakeLists.txt](src/TaxiVis/CMakeLists.txt:11)
- Sample dataset: `data/sample_merged_1.kdtrip` (10,000 trips from Jan 2013)

### Data Preprocessing

The `src/preprocess/` directory contains tools to index raw taxi CSV data into the `.kdtrip` binary format used by TaxiVis. See `doc/data_import.pdf` for preprocessing instructions.

## Architecture

### Core Data Structures

**KdTrip** ([KdTrip.hpp](src/TaxiVis/KdTrip.hpp))
- Central data structure for taxi trip storage and spatial queries
- Uses memory-mapped files (Boost iostreams) for efficient data access
- `Trip` struct contains pickup/dropoff times, locations, fare, distance, taxi ID, payment info
- `Query` struct supports range queries on time, location, and taxi ID
- `TripSet` uses boost::unordered_set for query results

**SelectionGraph** ([SelectionGraph.h](src/TaxiVis/SelectionGraph.h))
- Graph-based structure for managing spatial selections and trip filtering
- Nodes represent geographic regions (`SelectionGraphNode` wraps `Selection` geometry)
- Edges represent flow between regions
- Both nodes and edges are assigned color-coded `Group` objects for visualization
- Used by `QueryManager` to filter trips based on pickup/dropoff locations

**Selection** ([Selection.h](src/TaxiVis/Selection.h))
- Represents spatial regions using `QPainterPath` for arbitrary polygonal areas
- Supports three types: `START` (pickup), `END` (dropoff), `START_AND_END`
- Provides hit testing, translation, scaling operations for interactive manipulation

### Application Components

**Coordinator** ([coordinator.h](src/TaxiVis/coordinator.h))
- Singleton pattern for coordinating linked views
- Manages synchronization between multiple visualizations (map views, temporal plots, histograms)
- Handles shared state: plot attributes, camera position/zoom, time navigation

**QueryManager** ([querymanager.h](src/TaxiVis/querymanager.h))
- Bridges between `SelectionGraph` spatial queries and `KdTrip` data access
- Executes temporal + spatial queries and returns `TripSet` results

**MainWindow** ([mainwindow.h](src/TaxiVis/mainwindow.h))
- Primary Qt application window
- Manages UI layout and view composition
- Handles selection change events and dynamic map creation

### Rendering System

**RenderingLayer** ([RenderingLayer.hpp](src/TaxiVis/RenderingLayer.cpp))
- Abstract base class for OpenGL visualization layers
- Manages transitions between Qt painting and native OpenGL rendering
- Layers can be enabled/disabled for performance

**Layer Implementations** (`src/TaxiVis/layers/`):
- **GridMap**: Grid-based spatial aggregation visualization
- **HeatMap**: Density-based heat map rendering
- **TripAnimation**: Animated trip playback with VBO support
- **TripLocation**: Point-based trip location rendering using VBO
- **TripLocationLOD**: Level-of-detail optimized trip locations
- **Triangulator**: Geometry processing for spatial visualizations

**Shader Resources**
- GLSL shaders in `src/TaxiVis/Resources/shaders/`
- All shaders use GLSL version 1.20 (OpenGL 2.1)
- `geo2screen.120.vert`, `location.120.vert`, `paths.120.vert`, `paths.120.frag`

### Visualization Widgets

Qt-based widgets in `src/TaxiVis/`:
- **GeographicalViewWidget**: Map-based geographic visualizations
- **QMapView** / **QMapWidget**: Web-based map tile integration
- **HistogramWidget**: Statistical distributions
- **TemporalSeriesPlotWidget**: Time-series plotting
- **ScatterPlotWidget**: Correlation analysis
- **TimeSelectionWidget** / **TimeWidget**: Temporal navigation controls

All widgets use `qcustomplot` library for plotting functionality.

### Color Scales

Extensive color scale library in `src/TaxiVis/util/`:
- Multiple perceptually-uniform scales (divergent, sequential)
- Categorical scales (rainbow, pseudorainbow)
- Task-specific scales (heatedobject, grayscale variants)
- Extensible via `ColorScale` base class

## Running the Application

```bash
cd src/TaxiVis/build
./TaxiVis
```

The application loads the indexed trip data from the configured `DATA_DIR` and launches the Qt GUI with interactive map, temporal, and statistical views.
