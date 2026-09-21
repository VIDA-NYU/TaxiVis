# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

TaxiVis (also referred to as TLCVis) is a Qt6-based visual analytics application for exploring NYC taxi trip data. The application provides interactive visualization of taxi trips using OpenGL rendering, with support for spatial and temporal queries.

## Build System

### Building the Main Application

The project uses CMake (minimum 3.16) as its primary build system:

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH=/opt/homebrew/opt/qt
cmake --build build -j4
ctest --test-dir build --output-on-failure
```

The root build includes the GUI, preprocessing tools, and CTest regression suite.
Python 3 is required for tests; use `-DBUILD_TESTING=OFF` to omit them.
Use the Qt prefix appropriate to the host. Generated build directories are ignored.

### Preprocessing Tools

Executables: `csv2Binary`, `newFormatCsv2Binary`, `multiCsv2Binary`,
`build_kdtrip`, `sampling`, `testQuery`, `unif96_to_bin`.
`kdtrip_check` is built when testing is enabled and compares indexed results
against brute-force filtering. Julia scripts require CSV and DataFrames.

### Dependencies

- CMake (3.16+)
- Qt 6: Core, Gui, Widgets, OpenGL, OpenGLWidgets, Network, PrintSupport
- OpenGL/GLEW (2.2+)
- Boost (1.42+): iostreams, filesystem, timer components

## Data Management

### Data Location

- Default data directory: `data/` (relative to repository root)
- Configured via `DATA_DIR` macro in [CMakeLists.txt](src/TaxiVis/CMakeLists.txt:11)
- Sample dataset: `data/sample_merged_1.kdtrip` (10,000 trips from Jan 2013)

### Data Preprocessing

The `src/preprocess/` directory contains tools to index raw taxi CSV data into the `.kdtrip` binary format used by TaxiVis. See `README.md` and `doc/data_import.md` for preprocessing instructions.

## Architecture

### Core Data Structures

**KdTrip** ([KdTrip.hpp](src/TaxiVis/KdTrip.hpp))
- Central data structure for taxi trip storage and spatial queries
- Uses memory-mapped files (Boost iostreams) for efficient data access
- `Trip` is 56 bytes: times and coordinates, four uint32 custom fields, then taxi ID, distance, monetary fields, payment code, and passengers
- Loading validates the node layout and gathers statistics in one pass
- Inclusive queries must visit both subtrees when a bound equals the split median
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
- **QMapTileWidget** / **QMapWidget**: Native OpenStreetMap tile integration with memory/disk caching and Qt Network downloads
- **HistogramWidget**: Statistical distributions
- **TemporalSeriesPlotWidget**: Time-series plotting
- **ScatterPlotWidget**: Correlation analysis
- **TimeSelectionWidget** / **TimeWidget**: Temporal navigation controls

Statistical plot widgets use the vendored QCustomPlot 2.1.1.
See `src/TaxiVis/third_party/QCustomPlot.md` for provenance and the narrow
QtPrintSupport include patch. Keep application adapters outside the vendor code.
`plot_regression` tests production widgets offscreen using Qt6 Test;
`taxivis_widgets` is shared by the GUI and this test target.

### Color Scales

Extensive color scale library in `src/TaxiVis/util/`:
- Multiple perceptually-uniform scales (divergent, sequential)
- Categorical scales (rainbow, pseudorainbow)
- Task-specific scales (heatedobject, grayscale variants)
- Extensible via `ColorScale` base class

## Running the Application

```bash
TAXIVIS_DATA=/path/to/data.kdtrip ./build/src/TaxiVis/TaxiVis
```

The application loads the indexed trip data from the configured `DATA_DIR` and launches the Qt GUI with interactive map, temporal, and statistical views.

Dataset resolution: `TAXIVIS_DATA`, then `data/2012_merged.kdtrip`, then the bundled
sample. A local full January dataset is available at
`~/data/FOIL2013/processed/2013_01.kdtrip`; keep large datasets outside Git.

Qt 6 interactive verification is done by `native_interaction_check`
(`tests/native_interaction_check.cpp`): it drives the real window with in-process
Qt events and saves window grabs. It needs a display, GL and (on macOS) the
screen-recording permission, so it is a manual target, not a CTest case. Results
and the open merge decision are in `PLAN.md`. `make_figures` uses the same
approach to regenerate every screenshot under `doc/figs` (see `doc/figures.md`).

### Background computation

`AsyncTask.hpp` provides the GUI-owned latest-request controller (one active and
one pending task, cooperative cancellation, context-bound result delivery).
Workers must capture values only, never widgets or mutable SelectionGraph nodes.
`SelectionSnapshot.hpp` flattens geometry into value constraints; `KdTrip::TripSet`
shares immutable pointer storage and retains mapped/synthetic record owners.
Queries, plot aggregation/sorting, layers, exploration, and CSV serialization use
these snapshots. GUI/GL work stays on the main thread. CSV publication happens on
the GUI thread after cancellation checking; QSaveFile avoids partial destination
files. Attribute dialogs use owned snapshots and revision guards. Keep the
`responsiveness_regression` CTest suite current when changing these lifetimes.
