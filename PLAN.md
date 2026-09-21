# Plan: Qt 5 → Qt 6 port

Branch: `qt6`. Goal: build and run TaxiVis against Qt 6 (Homebrew `qt`,
6.11) with no change in behaviour, so the project stops depending on the
unmaintained `qt@5` keg. Replaces the old UPGRADE_PLAN.md, which covered the
Qt 4 → Qt 5 move that is now finished.

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

**qcustomplot** is version 1.0 (2012) and predates Qt 6. Patch it in place
rather than upgrade to 2.x, whose API for graph data and date axes differs
and would force a rewrite of all five plot widgets.

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
- Upgrading qcustomplot to 2.x.
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
- [ ] 5 runtime checks (January 2013 data)
  - [x] window opens, data loads, tiles render, points at the right place
  - [x] wheel zoom
  - [x] heat map lands on the trips
  - [x] rectangle and polygon regions, per-region time series
  - [x] attribute combo boxes connect (no "No such signal" at startup)
  - [ ] linking, delete, move, freehand
  - [ ] histogram and scatter tabs, Num Bins
  - [ ] time exploration and attribute exploration dialogs
  - [ ] time stepping, step size combo, Query button
  - [ ] export to CSV
  - [ ] second map, sync button
  - [ ] key A no-op
- [x] 6 preprocess tools build with Qt6
- [ ] 7 docs
- [ ] 8 merge
