# TaxiVis

A visual analytics application for exploring NYC taxi trip data using Qt6 and OpenGL.

## Current Status (Qt6 Migration)

**In progress:** The `qt6` branch builds against Qt 6. Core map functionality has been checked; the remaining interactive checks are tracked in [PLAN.md](PLAN.md).

## 1. Building from Source

### 1.1 Dependencies

**Required:**
- CMake 3.16 or higher
- Qt 6
- Qt6 modules: Core, Gui, Widgets, OpenGL, OpenGLWidgets, Network, PrintSupport, Concurrent
- OpenGL/GLEW 2.2+
- Boost 1.42+ (components: iostreams, filesystem, timer)

**macOS Installation (Homebrew):**
```bash
brew install cmake qt glew boost
```

**Linux Installation (Ubuntu/Debian):**
```bash
sudo apt-get install cmake qt6-base-dev libqt6opengl6-dev libglew-dev libboost-all-dev
```

### 1.2 Compiling with CMake

**Build everything (recommended; Python 3 is needed for tests):**
```bash
mkdir build
cd build

# macOS - set Qt6 path
export CMAKE_PREFIX_PATH="/opt/homebrew/opt/qt"

# Configure and build both TaxiVis and preprocessing tools
cmake ..
make -j4
```

This builds:
- `src/TaxiVis/TaxiVis` - Main visualization application
- `src/preprocess/csv2Binary` - CSV converter
- `src/preprocess/build_kdtrip` - KD-tree indexer
- `src/preprocess/multiCsv2Binary` - Batch CSV converter
- `src/preprocess/newFormatCsv2Binary` - Alternative CSV converter
- `src/preprocess/sampling` - Data sampling tool
- `src/preprocess/testQuery` - Query testing utility
- `src/preprocess/unif96_to_bin` - Legacy format converter

### 1.3 Build Troubleshooting

**Qt6 not found:** Set `CMAKE_PREFIX_PATH` to your Qt6 installation:
```bash
export CMAKE_PREFIX_PATH="/path/to/qt6"
# macOS Homebrew: /opt/homebrew/opt/qt
# Linux: /usr/lib/x86_64-linux-gnu/cmake/Qt6
```

**GLEW not found:** Ensure GLEW is installed via your package manager.

## 2. Running TaxiVis

### 2.1 Choosing a Dataset

TaxiVis loads one `.kdtrip` file at startup. It looks for the file in this order:

1. The path in the `TAXIVIS_DATA` environment variable, if set.
2. `data/2012_merged.kdtrip` (the output of `process_taxi_data.sh`), if present.
3. `data/sample_merged_1.kdtrip`, the bundled sample.

`data/` is the repository's `data` directory. Its location is compiled in via
the `DATA_DIR` macro in [CMakeLists.txt](src/TaxiVis/CMakeLists.txt) (line 11):
```cmake
add_definitions(-DDATA_DIR=\"${CMAKE_CURRENT_SOURCE_DIR}/../../data/\")
```

If the resolved file does not exist, TaxiVis prints the path it tried and exits.

**The bundled sample** (`data/sample_merged_1.kdtrip`) has 10,000 trips. It is
useful for checking that the build works, but it is not a representative slice
of a month: about 98% of its trips are from a single day (2013-01-13), with a
handful of trips scattered over the rest of January. Temporal plots will show
one spike. For real exploration, build a full month with the pipeline in
[Section 3](#3-data-preprocessing).

### 2.2 Executing

From the build directory, with the bundled sample:
```bash
./src/TaxiVis/TaxiVis
```

With your own dataset:
```bash
TAXIVIS_DATA=/path/to/2013_01.kdtrip ./src/TaxiVis/TaxiVis
```

On startup the console shows which file was loaded, the number of trips, and
the time range, for example:
```
Loading taxi trip data from: "/Users/you/data/FOIL2013/processed/2013_01.kdtrip"
Taxi trip data loaded successfully
  Number of trips: 14776615
  Time range: "2013-01-01 00:00" to "2013-02-01 10:33"
```

The `.kdtrip` file is memory-mapped. Startup validates its structure and collects
trip count and time range in one pass; this reads the full index, so startup time
and resident memory depend on dataset size and the filesystem cache. Invalid
node offsets and truncated records are rejected with an error.

### 2.3 Available Features

A walkthrough of the interface with screenshots is in [doc/usage.md](doc/usage.md), and [doc/figures.md](doc/figures.md) reproduces several figures from the TaxiVis paper with the 2013 data.

- **Geographic Map View** - OpenStreetMap tile-based visualization with pan/zoom
  - Three-tier caching: memory → disk → network
  - Persistent tile cache at `~/Library/Caches/TaxiVis/tiles` (macOS) or `~/.cache/TaxiVis/tiles` (Linux)
  - Keyboard controls: Arrow keys to pan, +/- to zoom
- **Temporal Series Plots** - Visualize trip metrics over time (fare, distance, duration, etc.)
- **Histograms** - Distribution analysis of trip attributes
- **Scatter Plots** - Correlation analysis between variables
- **Selection Graphs** - Define spatial/temporal query regions (rectangle, polygon, freehand) and link them for origin-to-destination queries; each region or link gets its own color in the plots
- **Heat Map Layer** - Density of pickups or dropoffs for the current query (key 1)
- **Time Exploration** - Split the query window into steps and compare them side by side, one map per step (Explore button in the time controls)
- **Attribute Exploration** - Plot every attribute of the current selection at once (Explore button on the toolbar)
- **Color Scales** - Multiple color schemes for data visualization
- **Data Export** - Query and export trip subsets

### 2.4 Known Limitations

- **Animated Trip Paths (TripAnimation)** - Unavailable on macOS: the layer needs a geometry shader, which the compatibility OpenGL profile used by the renderer does not provide. Toggling it (key A, toolbar button) does nothing there; pickup/dropoff points stay visible.
- **Grid Map layers** (keys 3 and 4) - Present but not verified in the current build
- **File → Open Dialog** - Not yet implemented; choose the dataset with `TAXIVIS_DATA` (see [Section 2.1](#21-choosing-a-dataset))

## 3. Data Preprocessing

TaxiVis requires taxi trip data in a specialized binary format (.kdtrip). The preprocessing pipeline converts raw CSV data into this indexed format for efficient querying.

### 3.1 Overview

The preprocessing workflow:
1. **Merge** - Combine trip and fare CSV files
2. **CSV → Binary** - Convert merged CSV to binary Trip format
3. **Binary → KdTrip** - Build KD-tree spatial index for fast queries

**Recommended:** Use the Julia-based pipeline for 10-15x faster processing (requires Julia 1.6+).
All preprocessing tools are in `src/preprocess/` (Julia) and `build/src/preprocess/` (C++).

### 3.2 Quick Start (Julia Pipeline - Recommended)

**Requirements:**
- Julia 1.6+ with the `CSV` and `DataFrames` packages (`julia -e 'using Pkg; Pkg.add(["CSV","DataFrames"])'`)
- The `build_kdtrip` tool from the CMake build (`build/src/preprocess/build_kdtrip`)
- Raw NYC TLC CSV files for 2013, which come as **pairs** per month:
  - `trip_data_N.csv`: medallion, hack license, vendor, pickup/dropoff times, passengers, distance, coordinates
  - `trip_fare_N.csv`: medallion, hack license, vendor, pickup time, payment type, fare, surcharge, tax, tip, tolls

  where `N` is the month (1 to 12). Both halves of a month are required: the
  trip file has the times and coordinates, the fare file has the money. A
  month is about 2.3 GB of trip data plus 1.6 GB of fare data, 14 to 15
  million trips.

**Processing one month by hand** (recommended; the three steps take a few
minutes on 16 cores):
```bash
RAW=/path/to/FOIL2013         # contains Tripdata_2013/ and Faredata_2013/
OUT=$RAW/processed
mkdir -p $OUT

# Step 1: Merge trip and fare CSVs on (medallion, hack_license, vendor_id, pickup_datetime)
julia -t 16 src/preprocess/merge.jl \
    $RAW/Tripdata_2013/trip_data_1.csv \
    $RAW/Faredata_2013/trip_fare_1.csv \
    $OUT/2013_01_merged.csv

# Step 2: Convert the merged CSV to the 56-byte binary Trip format
julia -t 16 src/preprocess/csv2Binary_mt.jl $OUT/2013_01_merged.csv $OUT/2013_01_merged.trip

# Step 3: Build the KD-tree index that TaxiVis loads
./build/src/preprocess/build_kdtrip $OUT/2013_01_merged.trip $OUT/2013_01.kdtrip

# Run it
TAXIVIS_DATA=$OUT/2013_01.kdtrip ./build/src/TaxiVis/TaxiVis
```

Intermediate files are large (the merged CSV is about 2 GB, the `.trip` file
about 800 MB) and can be deleted once the `.kdtrip` exists. Keep them out of
the repository; `data/*.csv`, `data/*.trip` and `data/*.kdtrip` are ignored by
git.

**Time zone.** Timestamps in the CSVs are New York wall-clock times. The
converter interprets them in the machine's local time zone, which is what
TaxiVis uses for queries and display. If you process data on a machine that is
not set to Eastern time, run the conversion with `TZ=America/New_York` in the
environment so that dates and hours in the application match the raw data:
```bash
TZ=America/New_York julia -t 16 src/preprocess/csv2Binary_mt.jl ...
```

**Automated script.** `process_taxi_data.sh` runs the same three steps with
timing output, but its input and output paths are hard-coded: it expects
`data/2012_trip_data_1.csv` and `data/2012_trip_fare_1.csv` and writes
`data/2012_merged.kdtrip`, which TaxiVis picks up automatically (see
[Section 2.1](#21-choosing-a-dataset)). Edit the variables at the top of the
script to point at other files.

**Multiple months.** Each month produces its own `.kdtrip`. TaxiVis loads a
single file, so switch datasets by changing `TAXIVIS_DATA`. To explore several
months in one dataset, merge each month (Step 1), concatenate the merged CSVs
(keeping only the first header line), and run Steps 2 and 3 on the combined
file. Run Step 2 once on the combined CSV rather than concatenating `.trip`
files: the converter assigns taxi and payment-type codes per run, so codes
from separately converted months would not agree.

### 3.3 C++ Preprocessing Tools (Legacy)

#### csv2Binary
Converts a single CSV file to binary Trip format.

**CSV Format Expected:**
```
pickup_time,dropoff_time,pickup_long,pickup_lat,dropoff_long,dropoff_lat,id_taxi,distance,fare_amount,surcharge,mta_tax,tip_amount,tolls_amount,payment_type,passengers,field1,field2,field3,field4
```

**Usage:**
```bash
./build/src/preprocess/csv2Binary input.csv output.trip
```

**Example:**
```bash
./build/src/preprocess/csv2Binary data/trips_2013-01.csv data/trips_2013-01.trip
```

#### multiCsv2Binary
Processes multiple CSV files listed in an index file. Useful for batch processing large datasets split across files.

**Usage:**
```bash
./build/src/preprocess/multiCsv2Binary file_list.txt output.trip
```

**file_list.txt format:**
```
/path/to/trips_2013-01-01.csv
/path/to/trips_2013-01-02.csv
/path/to/trips_2013-01-03.csv
```

#### newFormatCsv2Binary
Converts NYC taxi data in the newer format (post-2015) where column order differs.

**Usage:**
```bash
./build/src/preprocess/newFormatCsv2Binary input.csv output.trip
```

#### build_kdtrip
Builds a KD-tree spatial index from binary Trip data. This is the final step - produces the `.kdtrip` file that TaxiVis loads.

**Usage:**
```bash
./build/src/preprocess/build_kdtrip input.trip output.kdtrip
```

**Example:**
```bash
./build/src/preprocess/build_kdtrip data/trips_2013-01.trip data/trips_2013-01.kdtrip
```

**Output:** Creates a KD-tree index optimized for 7-dimensional queries:
- pickup_time
- dropoff_time
- pickup_longitude
- pickup_latitude
- dropoff_longitude
- dropoff_latitude
- taxi_id

#### sampling
Creates a spatially-filtered sample from a .kdtrip file. Filters trips by census tract geometry and time range.

**Usage:**
```bash
./build/src/preprocess/sampling input.kdtrip skip_factor output.csv
```

**Example (10% sample):**
```bash
./build/src/preprocess/sampling data/trips_2013.kdtrip 10 data/sample_10pct.csv
```

**Note:** Requires `data/census_tracts_geom.txt` geometry file.

#### testQuery
Tests KdTrip query functionality on a .kdtrip file. Useful for verifying index correctness.

**Usage:**
```bash
./build/src/preprocess/testQuery input.kdtrip
```

#### unif96_to_bin
Converts legacy 96-byte uniform binary format to the current Trip format. Only needed for old archived datasets.

**Usage:**
```bash
./build/src/preprocess/unif96_to_bin input.bin output.trip
```

#### merge.py
Python script to merge separate trip and fare CSV files into the unified format.

**Usage:**
```bash
python src/preprocess/merge.py trips.csv fares.csv merged.csv
```

### 3.4 Loading Data in TaxiVis

Point TaxiVis at any `.kdtrip` file with the `TAXIVIS_DATA` environment
variable, or drop it in `data/` as `2012_merged.kdtrip`. See
[Section 2.1](#21-choosing-a-dataset) for the full resolution order. No
rebuild is needed. A File → Open dialog is not implemented yet.

### 3.5 Data Format Reference

**Binary Trip Structure (56 bytes, in storage order):**
- `uint32_t pickup_time`, `dropoff_time` — Unix timestamps
- `float pickup_long`, `pickup_lat`, `dropoff_long`, `dropoff_lat` — coordinates
- `uint32_t field1, field2, field3, field4` — custom/region fields
- `uint16_t id_taxi` — taxi/medallion code
- `uint16_t distance` — hundredths of a mile
- `uint16_t fare_amount, surcharge, mta_tax, tip_amount, tolls_amount` — cents
- `uint8_t payment_type`, `passengers` — payment code and passenger count

The legacy `.trip` and `.kdtrip` formats use native byte order and have no
version header. Structural validation detects malformed layouts, but cannot
identify every incompatible or semantically corrupted dataset. Existing valid
indexes remain supported. Queries include both endpoints of each range.

## 4. Architecture

See [CLAUDE.md](CLAUDE.md) for detailed architecture documentation.

**Key Components:**
- **KdTrip** - Spatial indexing and query engine
- **QMapTileWidget** - Custom OpenStreetMap tile-based map widget with caching
- **SelectionGraph** - Graph-based spatial selection system
- **Rendering Layers** - OpenGL visualization (GridMap, HeatMap, TripAnimation, etc.)
- **QCustomPlot** - Plotting library for temporal/statistical views

## 5. Development

The current branch uses Qt 6, Boost, and GLEW, with a compatibility OpenGL
renderer and vendored QCustomPlot 2.1.1. The renderer still uses legacy
OpenGL APIs; the Qt migration does not remove that dependency. See [PLAN.md](PLAN.md)
for completed and outstanding runtime checks. Linux runtime behavior is unverified.

**Build and run automated data checks:**
```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH=/opt/homebrew/opt/qt
cmake --build build -j4
ctest --test-dir build --output-on-failure
```

Tests require Python 3 and Qt6 Test and are enabled by default (`-DBUILD_TESTING=OFF` disables
them). They exercise the real index builder, compare queries with brute-force
filtering, and check malformed-file rejection. Offscreen plot tests exercise
the production widgets: aggregation, bin edits, local-time labels, histogram
filtering, time-range selection, duplicate scatter coordinates, group colours,
and linked exploration axes. They use synthetic fixtures and the bundled sample.
The `testQuery` utility remains available for printing trips; it is not the regression suite.

To check a full January dataset without modifying it:
```bash
./build/src/preprocess/kdtrip_check ~/data/FOIL2013/processed/2013_01.kdtrip 14776615 full
```
The count argument must match the dataset. The `full` mode bounds query result
sizes while checking all seven indexed dimensions against full linear scans.

**QCustomPlot:** Version 2.1.1 is pinned in-tree. Its source URL, checksum,
license, and small QtPrintSupport include patch are recorded in
[src/TaxiVis/third_party/QCustomPlot.md](src/TaxiVis/third_party/QCustomPlot.md).
The plots use CPU painting; QCustomPlot's optional OpenGL backend is disabled.
Interactive queries, exploration batches, plot aggregation/sorting, and map-layer
preparation run on Qt Concurrent workers. Each consumer keeps one active request
and coalesces edits into one latest pending request. Immutable spatial/time and
copy-on-write trip snapshots isolate workers from live widgets; canceled results
cannot overwrite a newer selection. Closing a view cancels without waiting.

An “Updating selection…” indicator and disabled result actions distinguish an
in-flight query from committed results. CSV export runs against the committed
snapshot, writes a temporary file, and publishes atomically only if the view is
still open. Later selection edits do not change an export already in progress.
Exploration is cancellable and owns its generated, time-aligned trip storage.

Final QWidget painting and GL buffer/texture uploads remain on the GUI thread.
Very dense scatter plots can still take time to paint; this is distinct from the
background aggregation. Heat-map splats/colorization are computed into a QImage
on a worker, using source-over transmission instead of repeated per-ride GL draws;
minor pixel rounding differences from the former GPU pass are possible. Animation
routing uses the immutable road graph without its mutable shared path cache.

To check plots against the full local dataset and save review images:
```bash
QT_QPA_PLATFORM=offscreen TZ=America/New_York \
TAXIVIS_DATA="$HOME/data/FOIL2013/processed/2013_01.kdtrip" \
TAXIVIS_PLOT_OUTPUT="$PWD/build/plot-renders" \
./build/src/TaxiVis/plot_regression
```
This validates loading the full file and plots up to 10,000 trips from January 13;
it does not render all 14.8 million trips at once.

The `responsiveness_regression` suite covers request coalescing, stale-result
rejection, rapid query/bin edits, linked plots, layer replacement, committed CSV
exports, canceled exploration, stale attribute dialogs, and closing views during
work. It suppresses unsupported OpenGL viewport painting on the offscreen Qt
platform; it does not validate native GL drawing. Run it with the full file using:

```bash
QT_QPA_PLATFORM=offscreen TZ=America/New_York \
TAXIVIS_DATA="$HOME/data/FOIL2013/processed/2013_01.kdtrip" \
./build/src/TaxiVis/responsiveness_regression
```

## 6. References

- Original data source: http://www.andresmh.com/nyctaxitrips/
- Sample dataset: 10,000 trips from NYC Yellow Taxi, January 2013

## License

See original project documentation for license information.
