# QCustomPlot

Pinned version: **2.1.1**, released 2022-11-06.

Source: https://www.qcustomplot.com/release/2.1.1/QCustomPlot-source.tar.gz

Archive SHA-256: `5e2d22dec779db8f01f357cbdb25e54fbcf971adaee75eae8d7ad2444487182f`

The upstream amalgamated `qcustomplot.h` and `qcustomplot.cpp` are vendored in
`src/TaxiVis/`. Copyright and license notices are preserved; the distributed
GPL text is included in `QCustomPlot-GPL.txt`.

Local patch (header only): replace the umbrella `QtPrintSupport/QtPrintSupport`
include with `QtPrintSupport/QPrinter` and `QtPrintSupport/QPrintEngine`.
The umbrella header transitively imports Qt OpenGL headers before TaxiVis's
GLEW headers, producing fatal include-order errors. Narrow includes provide
all print types used by QCustomPlot without pulling in unrelated modules.
The `.cpp` is unchanged from upstream.

TaxiVis adapters belong in the application widgets, not in the vendored library.
QCustomPlot's optional OpenGL renderer remains disabled; map rendering is separate.

Scatter graphs explicitly disable adaptive sampling in application code. The
upstream sampler computes an infinite sampling stride when a dense pixel column
has zero value span, which trips Qt 6.11's checked rounding assertion. Rendering
all scatter points also preserves TaxiVis's alpha-blended density semantics.
Line graphs retain QCustomPlot's normal adaptive sampling behavior.
