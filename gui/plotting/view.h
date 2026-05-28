#ifndef GUI_PLOTTING_VIEW_H
#define GUI_PLOTTING_VIEW_H

#include "../../engine/expression/tree.h"
#include <wx/wx.h>

// Helper classes to limit variable count per object (Max 4 rule)
class PlotRange {
public:
  double xMin, xMax, yMin, yMax;
};

class PlotViewLayout {
public:
  int w, h, margin;
};

class BaseColors {
public:
  wxColour grid, axis, text;
};

class DataColors {
public:
  wxColour curve, bound, bar;
};

class PlotData {
public:
  engine::Node *root = nullptr;
  double a = 0.0, b = 0.0;
  int numBars = 10;
};

class PlotView : public wxPanel {
public:
  explicit PlotView(wxWindow *parent);
  ~PlotView() override; // Virtual destructor for dynamic memory cleanup

  void SetFunction(engine::Node *root, double a, double b);
  void ClearPlot();
  void SetNumBars(int n);

  // View controls. zoomFactor_ multiplies the visible span (1.0 = auto-fit);
  // panX_/panY_ shift the visible window in data-space units.
  void ZoomIn();
  void ZoomOut();
  void ResetView();

  // Coordinate conversion utilities (used by rendering helpers)
  int ToScreenX(double x, const PlotViewLayout &view, const PlotRange &range) const;
  int ToScreenY(double y, const PlotViewLayout &view, const PlotRange &range) const;

private:
  void OnPaint(wxPaintEvent &event);
  void OnResize(wxSizeEvent &event);
  void OnMouseDown(wxMouseEvent &event);
  void OnMouseUp(wxMouseEvent &event);
  void OnMouseMove(wxMouseEvent &event);
  void OnMouseLeave(wxMouseEvent &event);

  // Drawing helpers updated to use classes (Max 4 parameters)
  void DrawAxes(wxDC &dc, const PlotViewLayout &view, const PlotRange &range,
                const BaseColors &colors);
  void DrawFunction(wxDC &dc, const PlotViewLayout &view, const PlotRange &range,
                    const DataColors &colors);
  void DrawBars(wxDC &dc, const PlotViewLayout &view, const PlotRange &range, const wxColour &col);
  void DrawBoundLabels(wxDC &dc, const PlotViewLayout &view, const PlotRange &range,
                       const wxColour &col);

  PlotData data_; // Groups main state into 1 object
  bool hasFunction_ = false;

  // User view state (zoom & pan)
  double zoomFactor_ = 1.0; // 1.0 = auto-fit; >1 zoomed in; <1 zoomed out
  double panX_ = 0.0;       // shift of visible window centre in data space
  double panY_ = 0.0;

  // Pixels-per-data-unit recorded at the last paint, so the drag handler can
  // convert pixel deltas into data-space pan adjustments.
  double pixelsPerUnitX_ = 1.0;
  double pixelsPerUnitY_ = 1.0;

  // Drag-to-pan transient state.
  bool dragging_ = false;
  wxPoint dragStartPixel_;
  double dragStartPanX_ = 0.0;
  double dragStartPanY_ = 0.0;
};

#endif
