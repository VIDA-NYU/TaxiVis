# Plan: Qt 5 → Qt 6 port

Branch: `qt6`. Goal: build and run TaxiVis against Qt 6 (Homebrew `qt`,
6.11) with no change in behaviour, so the project stops depending on the
unmaintained `qt@5` keg. Replaces the old UPGRADE_PLAN.md, which covered the
Qt 4 → Qt 5 move that is now finished.

## Current stopping point

Done. The runtime checklist (step 5) is complete, the screenshots are
regenerated from the Qt 6 build, and the maintainer decided on 2026-09-21 that
the Qt 6 branch, including the asynchronous query layer as implemented, is the
better version and becomes `master` (step 8). The scope question under "Revised
responsiveness priorities" is thereby settled in favour of keeping the current
implementation; the measurements there remain the starting point for any future
responsiveness work (scatter painting first, not more threading).

## Findings (grep of src/TaxiVis, excluding qcustomplot)

| Qt 6 change | Uses | Action |
|---|---|---|
| `QDateTime::fromTime_t` / `toTime_t` removed | 6 | `fromSecsSinceEpoch` / `toSecsSinceEpoch` |
| `QButtonGroup::buttonClicked(int)` removed | 1 | `idClicked(int)` |
| `QOpenGLWidget` moved to the OpenGLWidgets module | 4 | link `Qt6::OpenGLWidgets` |
| `endl` on `QTextStream` | 5 | `Qt::endl` |
| `QMouseEvent::x()/y()/localPos()` deprecated | 25 | leave; compiles with warnings, change later |
| `foreach`, `QVector` | 51 | leave; still supported |
| Fixed-function OpenGL (GL 2.1 compatibility) | 19 | none; Qt 6 does not change the GL profile |

Not present: QRegExp, QDesktopWidget, QTextCodec, QLinkedList, QStringRef,
QGL*, QMatrix.

**Original QCustomPlot decision (superseded):** The initial Qt 6 port retained
and patched the 2012 library. The authorized QCustomPlot 2 migration below now
replaces it with pinned 2.1.1 and migrates the application APIs.

## Steps

1. **CMake.** `cmake_minimum_required(3.16)`. `find_package(Qt6 COMPONENTS
   Core Gui Widgets OpenGL OpenGLWidgets Network PrintSupport)`. Replace
   `qt5_wrap_ui` with `qt_wrap_ui` (or drop it, AUTOUIC already runs).
   Link `Qt6::OpenGLWidgets`. README: `brew install qt` instead of `qt@5`,
   `CMAKE_PREFIX_PATH=/opt/homebrew/opt/qt`.
2. **Mechanical API fixes** in the app: the four rows marked with an action
   above.
3. **qcustomplot 1.0**: fix whatever the compiler reports (expected:
   `fromTime_t`, QPrinter internals, `QString` / `QVariant` conversions).
4. **Build** against Qt 6, fix remaining errors until clean.
5. **Runtime checks**, each against the January 2013 dataset:
   - window opens, map tiles load and cache, pan/zoom, arrow keys
   - point layer draws at the right place and size at zoom 11, 12, 15
   - heat map (key 1) lands on the trips (depends on the painter transform
     being in the modelview matrix during native painting; Qt 6 may differ)
   - rectangle, polygon, freehand regions; linking; delete; move
   - time series, histogram, scatter tabs; Num Bins edit
   - time exploration dialog and both attribute exploration dialogs
   - Explore/Query buttons, time stepping, step size combo
   - export to CSV
   - a second map via Views → New Map, and the sync button
   - key A remains a no-op on macOS
6. **Preprocess tools** build unchanged (they do not use Qt except
   `csv2Binary`, which uses QDateTime; check it compiles).
7. **Docs.** README dependency and build sections, CLAUDE.md.
8. Merge to `master` once every runtime check passes on macOS. Linux stays
   untested unless someone has a box.

## Out of scope

- Replacing fixed-function OpenGL with a core-profile renderer.
- QCustomPlot 2.x was originally excluded; now authorized and tracked below.
- Fixing the deprecated mouse-event accessors (warnings only).
- TripAnimation on macOS (needs a geometry-shader-free implementation).

## Status

Updated as steps land.

- [x] 1 CMake: Qt6 with OpenGLWidgets, cmake 3.16, preprocess tools too
- [x] 2 API fixes in the app (see commit log; also `register`, QFontMetrics::width,
      QTime as a stopwatch, QPolygonF iterators, QWheelEvent::orientation,
      QComboBox `currentIndexChanged(QString)` in two .cpp and three .ui files)
- [x] 3 qcustomplot 1.0 patched in place (QMap::insertMulti/unite, QPrinter
      page size, removed render hints, QWheelEvent::delta/pos, QSet::toList,
      QLocale exponential/positiveSign now QString)
- [x] 4 clean build against Qt 6.11.2, zero warnings, links Qt6 frameworks
- [x] 5 runtime checks (January 2013 data) — see "Native interaction checks" below
  - [x] window opens, data loads, tiles render, points at the right place
  - [x] wheel zoom
  - [x] heat map lands on the trips
  - [x] rectangle and polygon regions, per-region time series
  - [x] attribute combo boxes connect (no "No such signal" at startup)
  - [x] linking, delete, move, freehand
  - [x] histogram and scatter tabs, Num Bins
  - [x] time exploration and attribute exploration dialogs
  - [x] time stepping, step size combo, Query button
  - [x] export to CSV (code behind the button; the native save dialog is not driven)
  - [x] second map, sync button
  - [x] key A no-op
- [x] 6 preprocess tools build with Qt6
- [x] 7 docs (Qt 6 setup, tests, 56-byte binary layout, startup validation)
- [x] 8 merge (`master` fast-forwarded to `qt6`, 2026-09-21)

## Repository review fixes (2026-09-21)

Data correctness, validation, documentation, and repository cleanup completed.
Interactive migration checks and asynchronous UI work remain tracked separately.

- [x] Fix inclusive KD-tree query boundaries when duplicate split values appear
      in the right subtree; preserve compatibility with existing indexes.
- [x] Register automated CTest regression checks comparing indexed results with
      brute-force filtering, including duplicate values and all seven dimensions.
- [x] Validate binary index structure before traversal and report load failures
      clearly; test truncated files, invalid offsets, cycles, and leaf payloads.
- [x] Harden index creation for empty, partial, single-record, and duplicate input.
- [x] Assess synchronous query/startup work; remove redundant startup scanning
      and record any remaining responsiveness work explicitly.
- [x] Remove generated `build-qt6/` files from Git tracking while retaining the
      local build, and ignore build directories.
- [x] Update README and CLAUDE.md for Qt 6 and the actual 56-byte binary format.
- [x] Run a fresh build and automated checks; update the runtime checklist with
      only checks actually performed. Do not merge as part of this repair work.

### Verification results

- Fresh Release build in `build-review/` succeeds. The installed Qt/Boost emit
  existing deprecation/configuration warnings; this is not a zero-warning build.
- `ctest --test-dir build-review --output-on-failure`: passes. Fixtures cover
  single records, duplicate pairs, 4,096 identical records, mixed data, malformed
  indexes, builder failures, and the bundled 10,000-trip sample.
- Existing full dataset `~/data/FOIL2013/processed/2013_01.kdtrip` is accepted:
  14,776,615 trips; all 65 comparison queries match brute-force filtering. The
  source index, merged CSV, and `.trip` file were not modified.
- Startup statistics now come from the structural-validation pass, avoiding a
  second scan. Validation intentionally reads the whole index; it is not O(1).
- AddressSanitizer + UndefinedBehaviorSanitizer build of the builder/checker:
  regression suite passes.
- Repaired GUI launched with the full January dataset, reports 14,776,615 trips
  and the expected time range, and reaches first paint/OpenGL initialization.
  The UI automation tool cannot attach to this unbundled executable (reports
  `Invalid app`), so this is a launch smoke check, not visual verification of
  the outstanding interactive checklist. Those boxes remain unchecked.
- Rebuilt the full January `.trip` into `build-review/2013_01_review.kdtrip`:
  14,776,615 trips, with all 65 comparison queries matching brute-force filtering.
  The rebuilt index is a separate ignored build artifact; originals are unchanged.
- User confirmed the repaired application is visibly running.

### Responsiveness work

- [x] Move interactive queries and exploration batches off the UI thread using
  immutable selection/time snapshots, cooperative cancellation, coalescing, and
  stale-result rejection. Plot aggregation and layer preparation also run in
  workers. Integration checks cover rapid edits, linked views, export,
  exploration, and closing a view during work. See implementation notes below.

## QCustomPlot 2 migration (2026-09-21)

Authorized follow-up; supersedes the earlier decision to retain QCustomPlot 1.0.

- [x] Vendor upstream QCustomPlot 2.1.1 with version/source/checksum provenance.
- [x] Migrate time series, histogram, scatter, and custom interaction subclasses
      to the 2.x data, ticker, axis-rectangle, style, and selection APIs.
- [x] Build with the installed Qt 6 and document any required upstream patches.
- [x] Add plot regression checks for aggregation, bins, labels, and selection
      behavior; exercise the actual widgets with January data.
- [x] Run data and plot tests, inspect rendered plots, and update documentation.

Migration notes:
- Vendored 2.1.1 with source URL, archive SHA-256, and GPL text. One header
  patch narrows QtPrintSupport includes to avoid the Qt/GLEW include conflict;
  upstream `qcustomplot.cpp` is unchanged.
- Date/text tickers, bar containers, scatter styles, title layout elements,
  axis rectangle interactions, and exploration-dialog axis linking migrated.
- Removed obsolete manual time-label construction (its helper returned an
  uninitialized buffer) and a redundant nested graph rebuild loop. The new
  date/time ticker formats local-time labels directly.
- Production widgets are shared by the GUI and the new offscreen Qt Test
  executable, covering counts, bin edits, titles/tickers, histogram filtering,
  right-drag time selection, axis restrictions, duplicate scatter keys, grouped
  plots, and exploration-dialog axis synchronization. CTest passes both suites.
- Initial full-file plot run loads 14,776,615 trips and checks/render-inspects a
  bounded 10,000-trip January 13 subset (not the entire month in one plot).
- Histogram bin setters now synchronize the displayed spin box with the actual
  bin count; title rows and plottable ownership remain stable across replots.
- Sanitizer/Debug checks exposed QCP 2.1.1's default adaptive scatter sampler
  rounding an infinite stride for coincident points. Disabled scatter adaptive
  sampling in application code, preserving alpha-blended density and avoiding
  this assertion without modifying the upstream implementation.

Final migration verification:
- Release and AddressSanitizer/UndefinedBehaviorSanitizer Debug builds both
  pass `kdtrip_regression` and `plot_regression` (2/2 tests in each build).
- Full January file loaded again after the scatter fix; synthetic fixtures,
  linked exploration dialogs, and the 10,000-trip real-data subset pass.
- Inspected time-series, histogram, and scatter PNGs in
  `build-review/plot-renders/`; confirmed date labels, titles, bin labels/count,
  and preserved scatter density. These are offscreen widget renders, not a
  replacement for the remaining native map-interaction checklist above.
- README and CLAUDE.md describe QCustomPlot 2.1.1, provenance, tests, and the
  full-dataset rendering command. Upstream source verified byte-for-byte except
  for the documented header include patch (upstream whitespace preserved).
- Launched the upgraded native GUI: startup reports Qt 6.11.2 / QCustomPlot
  2.1.1, loads all 14,776,615 January trips, and reaches first paint/OpenGL
  initialization. The earlier running instance was left intact.

## Responsiveness implementation (2026-09-21)

- [x] Add immutable, cheaply shared trip/selection snapshots and a cancellable
      job runner with one active and one coalesced pending request per consumer.
- [x] Background interactive queries; reject superseded results and expose busy
      state so exports/exploration cannot mistake old data for the new selection.
- [x] Background temporal/histogram/scatter aggregation and layer preparation;
      keep QWidget operations and OpenGL uploads on the GUI thread.
- [x] Background exploration queries and CSV export; own all data used after a
      dialog/view closes, and remove nested processEvents-based work loops.
- [x] Add deterministic integration checks for cancellation, rapid edits, linked
      views, export snapshots, exploration, and destruction during active jobs.
- [x] Validate Release/sanitizer builds and the full January dataset; document
      the remaining cost of final rendering/uploads separately from computation.

Implementation in place: shared trip storage, value-only spatial constraints,
latest-request controllers, background query/plot/layer preparation, cancellable
exploration, atomic CSV publication, and dialog revision guards. QWidget changes
and GL uploads remain on the GUI thread. Release and sanitizer regression
validation is recorded below.

Validation and scope:
- Release: all three CTest suites pass (data, plotting, responsiveness).
- ASan/UBSan Debug: all three suites pass with leak detection disabled for Qt's
  process-lifetime allocations; this is not a leak audit.
- Responsiveness integration also passes with the original 14,776,615-trip January
  index. It loads the entire file and exercises bounded one/two-hour selections,
  rapid edits, exports, exploration, and canceled broad queries; it does not
  render the entire month as a scatter plot.
- Tests cover immutable spatial edge constraints after the source graph is
  destroyed, copy-on-write trip storage, one latest pending request, failure
  delivery, stale-result rejection, nonblocking destruction, empty/replaced layer
  data, heat normalization, linked plot ranges, stable CSV snapshots, stale
  histogram dialogs, canceled exploration, and close-during-query/export.
- Offscreen tests suppress unsupported GL viewport paints. Native OpenGL map
  appearance is not revalidated by these tests. The rebuilt executable is ready;
  existing running app instances were not restarted.
- QWidget painting and GL uploads remain on the GUI thread. Large scatter plots
  can still be expensive to draw; their data grouping/sorting/range scans now run
  in workers. Heat-map rasterization uses equivalent source-over transmission,
  with possible pixel-rounding differences from the previous GPU splat pass.
- Animation preparation uses uncached read-only shortest paths to avoid races in
  the road map's mutable shared cache. Startup dataset validation remains
  synchronous and is outside this interactive-query work.

## Responsiveness review (2026-09-21)

Independent review of the section above, after it was marked complete. The
conclusion is that the threading is not justified by measured latency, and that
one concrete data race was introduced. Measurements are from the full January
index (`~/data/FOIL2013/processed/2013_01.kdtrip`, 14,776,615 trips) on this
host, Release `-O2`.

### Measured cost of the work being moved off the UI thread

Index queries (`KdTrip::execute`, best of three):

| Query | Time | Trips |
|---|---|---|
| 1 hour, whole city | 17 ms | 29,283 |
| 1 hour, midtown bbox | 4 ms | 4,028 |
| 1 day, whole city | 18 ms | 412,706 |
| 1 week, whole city | 40 ms | 3,010,260 |
| full month, whole city | 141 ms | 14,776,615 |

Per-trip work layered on top: `QPainterPath::contains()` costs ~0.01 us/trip
(500k points in 2.9 ms for a rectangle, 6.5 ms for a 40-point polygon), and
`boost::unordered_set` insertion is 7.4 ms per 500k. A typical interaction —
an hour window against a drawn region — is therefore ~20-30 ms end to end.

Exploration batches (N sequential hourly queries):

| Batch | Time |
|---|---|
| 8 ranges | 132 ms |
| 24 ranges | 386 ms |
| 48 ranges | 751 ms |
| 168 ranges | 2.5 s |

`ViewWidget::exploreInTime` already gates this: above 7 cells it asks for
confirmation, so the unprompted path is ~130 ms and anything slower is
explicitly opted into with a progress dialog on screen.

### The processEvents hazard did not require async

Both `QApplication::processEvents()` call sites at HEAD were real reentrancy
hazards, but neither needed a job runner to fix:

- `viewwidget.cpp:322` (exploration): the loop already operates on a local copy
  (`selectionGraph.assign(...)`), and the progress dialog is constructed with
  `QString()` as its cancel-button text, so no cancel button is shown. The call
  bought only progress-bar repainting during a ~130 ms operation. Deleting the
  line makes the loop atomic; nothing observable is lost.
- `layers/TripAnimation.cpp:351`: genuinely dangerous — one `processEvents()`
  per trip while iterating a live `TripSet*` obtained from the widget, which can
  invalidate the iterator mid-loop, plus seconds of pure call overhead at 400k
  trips. This is the one loop expensive enough to warrant background work, but
  TripAnimation is listed as out of scope and non-functional on macOS, so it is
  not a reason to build this infrastructure now.

### End-to-end measurement (closes the evidence gap above)

The first review measured only the query path, so its 20-30 ms figure did not
cover plot aggregation or painting. `tests/interaction_bench.cpp` (built as
`interaction_bench`, not a CTest case) measures every stage. Full January index,
Release, offscreen, 1000x420 widgets, 10 bins, ms:

| scenario | trips | query | temporal | histogram | scatter | paint |
|---|---|---|---|---|---|---|
| 1 hour, whole city | 26,806 | 4.0 | 17.4 | 14.9 | 25.7 | 100.3 |
| 1 hour, drawn region | 4,162 | 1.2 | 16.4 | 15.0 | 17.9 | 20.9 |
| 1 day, whole city | 440,581 | 24.8 | 16.8 | 60.6 | 298.3 | 1539.4 |
| 1 day, drawn region | 73,828 | 7.4 | 16.8 | 15.8 | 62.7 | 266.3 |

The agent's objection was fair: aggregation is not free — scatter aggregation
reaches 298 ms on a day of trips. But the conclusion still holds, for a stronger
reason than the original review gave. **Painting dominates every scenario, and
painting is explicitly left on the GUI thread.** On the heaviest selection the
threading moves ~376 ms of aggregation off the UI thread while 1539 ms of
painting stays on it: the freeze goes from ~1.9 s to ~1.5 s. On the typical
selection it hides ~60 ms of a ~160 ms interaction. Threading never produces a
responsive UI here, because it targets the minority of the cost.

The dominant cost is scatter rendering, and there is a far larger lever there.
Forced scatter replot, same runs:

| scenario | replot as-is | replot with adaptive sampling |
|---|---|---|
| 1 hour, whole city | 47.4 ms | 0.6 ms |
| 1 day, whole city | 770.4 ms | 3.0 ms |
| 1 day, drawn region | 131.3 ms | 0.9 ms |

Adaptive sampling was disabled during the QCustomPlot migration to avoid an
upstream assertion on coincident points. Re-enabling it is not free — it drops
overlapping points and so changes the alpha-blended density the scatter plot
uses to convey volume — but it is a ~250x lever on the dominant cost, versus
~20% for threading. Any future responsiveness work should start here, not with
worker threads. (Measured by forcing `replot(rpImmediateRefresh)`; a plain
`grab()` blits QCustomPlot's cached buffer and reports a misleading 0.3 ms.)

### Data race introduced: shared QPainterPath in SelectionSnapshot — FIXED

- [x] `SpatialConstraint` stored `QPainterPath pickup, dropoff` by value
      (`SelectionSnapshot.hpp:9`) and calls `contains()` from worker threads
      (`:12`). `QPainterPath` is implicitly shared, so every worker copy points
      at the same private as the live `Selection` geometry on the GUI thread.
      `contains()` populates a lazily computed bounds cache through a `const`
      method: on a fresh 2000-point path the first call costs ~11-14 us versus
      ~7.4 us for subsequent calls, and pre-warming with the *const*
      `controlPointRect()` removes the penalty entirely — confirming the write
      to shared state. The copy-on-write refcount is atomic; this cache is not.
      Concurrent workers, or one worker while the GUI thread hit-tests on
      mouse-move, race on it.

      Fix: flatten to `QPolygonF` via `toFillPolygon()` once in `capture()` on
      the GUI thread and use `containsPoint()` in workers. Polygons have no lazy
      cache, and selections are small enough that the deep copy is free.

      Note: a ThreadSanitizer run over this pattern came back clean, but that is
      not evidence of safety — the Homebrew Qt frameworks are prebuilt and
      uninstrumented, so TSan cannot see inside `contains()`.

      Fixed: `SelectionSnapshot.hpp` now flattens geometry to a `Region`
      (`QPolygonF` + fill rule) at capture time on the GUI thread, and workers
      call `QPolygonF::containsPoint`/`boundingRect`, neither of which caches.
      Selections are built only from `addRect`/`addPolygon`/`moveTo`/`lineTo`
      (verified in `geographicalviewwidget.cpp`), so `toFillPolygon()` is exact
      here rather than a curve approximation. `snapshotChecks` in
      `tests/responsiveness_regression.cpp` gained a containment-equivalence
      check over a probe grid, including a non-convex outline where the fill
      rule matters; all three CTest suites pass on the bundled sample and on the
      full January index.

### Recommendation

- [ ] Keep `SelectionSnapshot` and the `querymanager` simplification
      (`querymanager.cpp` is +36/-195); replacing a walk over a mutable
      `SelectionGraph` with an immutable captured value is better design
      independent of threading, and is what earned most of the deletions.
- [ ] Delete the two `processEvents()` calls, as already done.
- [ ] Reconsider backgrounding plot aggregation and layer preparation (item 3
      above). That work is the ~20-30 ms case, below the perception threshold,
      and it is where the race surface is bought for no visible gain.
- [ ] If the threading is kept: fix the `QPainterPath` sharing above; delay
      busy-state indicators by ~150 ms so they do not strobe during drags at
      20 ms per query; and consider a dedicated `QThreadPool`, since
      `QtConcurrent::run` uses the global pool where a full-month job can
      head-of-line block short ones.

Cost of not threading: ~25 ms on interaction (invisible), ~130 ms on typical
exploration, up to ~2.5 s on user-confirmed large batches, ~0.5 s on a
full-month selection. Cost of threading: a class of race the synchronous code
cannot have, on a codebase whose plot layer was stabilized the same day and
whose native map-interaction checklist is still unchecked.

## Revised responsiveness priorities and next steps

Agreed stopping point: review the scope before making further code changes.
No rollback or additional threading work has been performed as part of this
plan update.

- Keep the data-correctness repairs, simplified query implementation, immutable
  selection snapshots, and removal of nested `processEvents()` calls.
- Reconsider background interactive queries, plot aggregation, and layer
  preparation. Retain threading only where measured end-to-end interaction cost
  demonstrates a useful benefit.
- Consider retaining background CSV export and large exploration batches, where
  long operations have a clearer justification. This is a recommendation, not a
  decision to retain every part of the current implementation.
- Defer a dedicated thread pool and further busy-indicator machinery. Neither is
  required work without evidence that it solves a noticeable problem.
- Avoid extending animation threading as part of this scope review; macOS
  animation remains outside the original migration goal.

Required before treating the asynchronous implementation as merge-ready:

- [x] Agree on the smaller scope to retain or simplify. **Decided 2026-09-21:
      keep the implementation as is.** This was a judgement call, not a measurement. The evidence now
      says threading addresses ~20% of the worst-case interaction cost while
      painting accounts for the rest; the code is written, tested, and no longer
      carries the known race. Retaining it is defensible; so is reverting plot
      aggregation and layer preparation to synchronous and keeping only the
      snapshot, the simplified `querymanager`, and background exploration/export.
- [x] Measure representative end-to-end interactions, including query result
      materialization, plot aggregation/sorting, layer preparation, and final
      painting/uploads. See the end-to-end table above; `interaction_bench`
      reproduces it.
- [x] Resolve the reported `QPainterPath` sharing concern wherever workers still
      use selection geometry. Verify the chosen representation preserves
      selection semantics, including fill rules and curved paths if supported.
- [ ] Implement only the justified simplifications/fixes, then rerun the relevant
      regression tests and focused native interaction checks before merging.

Evidence boundary: the appended review measures index queries, path containment,
set insertion, and exploration query batches. It does not separately establish
plot aggregation, sorting, heat-map preparation, or rendering costs. Its blanket
20–30 ms conclusion for those operations is therefore not established. Conversely,
the asynchronous implementation's passing tests do not demonstrate that its
additional complexity produces a worthwhile user-visible improvement.

## Native interaction checks (2026-09-21)

`tests/native_interaction_check.cpp` (target `native_interaction_check`, not a
CTest case because it needs a display and OpenGL) runs the real `MainWindow` on
the host platform and drives it with in-process Qt events, so it neither needs
an accessibility tool nor moves the user's cursor. Every step asserts on
application state (selection graph, committed trip set versus a synchronous
reference query, widget values) and saves a window-system grab of the native
window. `QWidget::grab()` and `QOpenGLWidget::grabFramebuffer()` both return a
blank map because the scene paints natively into the viewport, so the grabs use
`QScreen::grabWindow` (needs the macOS screen-recording permission).

    TAXIVIS_DATA=~/data/FOIL2013/processed/2013_01.kdtrip \
        build/src/TaxiVis/native_interaction_check out-dir

Result: 17/17 pass on the full January index and on the bundled sample. Covered:
Query button; rectangle (geometry matches the dragged corners), freehand, move,
link, Ctrl-click delete (edge removed with its node); histogram/scatter/time
series tabs and typed Num Bins edits; step buttons with 1 hour and 15 min step
sizes; arrow-key stepping from the map; key A no-op on macOS; CSV export row
count; both attribute exploration dialogs; time exploration (two cells);
Views > New Map with sync (zoom, centre and time follow). Grabs were inspected:
regions, link arrow, pickup/dropoff points, exploration heat maps and the linked
second map all render in the right place.

Found and fixed along the way:

- Linked views advanced **two** steps per arrow key: the key handler notified
  linked widgets explicitly and then forwarded the same key event to them. The
  explicit notification (`notifyCoordinatorStepBack/Forward`) is removed. This
  dates from 2015 and is not a Qt 6 regression.
- `main.cpp` resized the window to 1280x800 after the constructor, overriding
  the new screen-relative size and leaving the centring computed for the wrong
  size. The override is removed. The splitter now reserves a readable plot
  panel (minimum height, 2:1 initial split).
- `responsiveness_regression.cpp` included `ViewWidget.h`; the file is
  `viewwidget.h`, which only worked on a case-insensitive filesystem.

Noticed, not changed: the step *buttons* are per-view by design (only the map's
arrow keys propagate to linked views); linked views share a time-series y range,
so a small selection looks flat next to a large one; with two maps on a
1512-point-wide display the views' minimum widths slightly exceed the window;
the colour-bar label is clipped in short exploration cells.

