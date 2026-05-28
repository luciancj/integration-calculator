#include "view.h"
#include <algorithm>
#include <cmath>
#include <sstream>
#include <utility>
#include <vector>
#include <wx/dcbuffer.h>
#include <wx/dcgraph.h>
#include <wx/settings.h>

namespace {
constexpr int kYRangeSamples = 500;

DataColors BuildDataColors(const wxColour &accent) {
  return {accent, accent,
          wxColour(accent.Red(), accent.Green(), accent.Blue(), 40)};
}

void DrawEmptyPlotPlaceholder(wxDC &dc, int width, int height, const wxColour &textColor) {
  wxFont messageFont = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
  messageFont.SetStyle(wxFONTSTYLE_ITALIC);
  dc.SetFont(messageFont);
  dc.SetTextForeground(textColor);

  wxString message = wxString::FromUTF8("Enter a function and press Calculate");
  wxSize messageSize = dc.GetTextExtent(message);
  int textX = (width - messageSize.GetWidth()) / 2;
  int textY = (height - messageSize.GetHeight()) / 2;
  dc.DrawText(message, textX, textY);
}

std::pair<double, double> ComputeXRange(const PlotData &data) {
  double xPadding = (data.b - data.a) * 0.15;
  if (xPadding < 0.5) {
    xPadding = 0.5;
  }

  double xMin = data.a - xPadding;
  double xMax = data.b + xPadding;
  return {xMin, xMax};
}

std::vector<double> CollectFiniteYValues(const engine::Node *root, double xMin, double xMax) {
  std::vector<double> values;
  values.reserve(kYRangeSamples + 1);

  for (int i = 0; i <= kYRangeSamples; ++i) {
    double t = 1.0 * i / kYRangeSamples;
    double x = xMin + (xMax - xMin) * t;
    double y = root->evaluate(x);

    if (std::isfinite(y)) {
      values.push_back(y);
    }
  }

  return values;
}

std::pair<double, double> ComputeRobustYBounds(std::vector<double> yValues) {
  if (yValues.empty()) {
    return {0.0, 0.0};
  }

  std::sort(yValues.begin(), yValues.end());

  int q1Index = yValues.size() * 0.25;
  int q3Index = yValues.size() * 0.75;
  double q1 = yValues[q1Index];
  double q3 = yValues[q3Index];
  double iqr = q3 - q1;

  double lowerBound = q1 - 1.5 * iqr;
  double upperBound = q3 + 1.5 * iqr;

  if (upperBound - lowerBound < 2.0) {
    lowerBound -= 1.0;
    upperBound += 1.0;
  }

  return {lowerBound, upperBound};
}

std::pair<double, double> AddVisibilityPadding(double yMin, double yMax) {
  if (yMin > 0.0) {
    yMin = 0.0;
  }

  if (yMax < 0.0) {
    yMax = 0.0;
  }

  double yPadding = (yMax - yMin) * 0.12;
  if (yPadding < 0.1) {
    yPadding = 0.1;
  }

  yMin -= yPadding;
  yMax += yPadding;
  return {yMin, yMax};
}

void EnforceUnitScaleOneToOne(double &xMin, double &xMax, double &yMin, double &yMax,
                              int drawableWidth, int drawableHeight) {
  if (drawableWidth <= 0 || drawableHeight <= 0) {
    return;
  }

  double xSpan = xMax - xMin;
  double ySpan = yMax - yMin;
  if (xSpan <= 0.0 || ySpan <= 0.0) {
    return;
  }

  double plotAspect = 1.0 * drawableWidth / drawableHeight;
  double currentAspect = xSpan / ySpan;

  // Strict 1:1 unit scale means pixels-per-unit must match on both axes.
  // Therefore, xSpan / ySpan must equal drawableWidth / drawableHeight.
  if (currentAspect > plotAspect) {
    double newYSpan = xSpan / plotAspect;
    double yCenter = (yMin + yMax) / 2.0;
    yMin = yCenter - newYSpan / 2.0;
    yMax = yCenter + newYSpan / 2.0;
  } else if (currentAspect < plotAspect) {
    double newXSpan = ySpan * plotAspect;
    double xCenter = (xMin + xMax) / 2.0;
    xMin = xCenter - newXSpan / 2.0;
    xMax = xCenter + newXSpan / 2.0;
  }
}

void DrawPlotTitleAndSubtitle(wxDC &dc, int width, const PlotData &data, const wxColour &titleColor,
                              const wxColour &subtitleColor) {
  wxFont titleFont = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
  titleFont.SetWeight(wxFONTWEIGHT_BOLD);
  dc.SetFont(titleFont);
  dc.SetTextForeground(titleColor);

  std::ostringstream titleStream;
  titleStream << "f(x) = " << data.root->toString();
  wxString title(titleStream.str());
  wxSize titleSize = dc.GetTextExtent(title);
  int titleX = (width - titleSize.GetWidth()) / 2;
  dc.DrawText(title, titleX, 12);

  wxFont subtitleFont = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
  dc.SetFont(subtitleFont);
  dc.SetTextForeground(subtitleColor);
  std::ostringstream subtitleStream;
  subtitleStream << "Interval [" << data.a << ", " << data.b << "]";
  wxString subtitle(subtitleStream.str());
  wxSize subtitleSize = dc.GetTextExtent(subtitle);
  int subtitleX = (width - subtitleSize.GetWidth()) / 2;
  int subtitleY = 12 + titleSize.GetHeight() + 2;
  dc.DrawText(subtitle, subtitleX, subtitleY);
}
} // namespace

PlotView::PlotView(wxWindow *parent)
    : wxPanel(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxFULL_REPAINT_ON_RESIZE) {
  SetBackgroundStyle(wxBG_STYLE_PAINT);

  Bind(wxEVT_PAINT, &PlotView::OnPaint, this);
  Bind(wxEVT_SIZE, &PlotView::OnResize, this);
  Bind(wxEVT_LEFT_DOWN, &PlotView::OnMouseDown, this);
  Bind(wxEVT_LEFT_UP, &PlotView::OnMouseUp, this);
  Bind(wxEVT_MOTION, &PlotView::OnMouseMove, this);
  Bind(wxEVT_LEAVE_WINDOW, &PlotView::OnMouseLeave, this);
}

PlotView::~PlotView() {
  delete data_.root;
}

void PlotView::SetFunction(engine::Node *root, double a, double b) {
  if (data_.root != root) {
    delete data_.root;
    data_.root = root;
  }
  data_.a = a;
  data_.b = b;
  hasFunction_ = true;
  // A new calculation: snap back to the auto-fit view so the user sees the
  // result framed with its integration bounds.
  ResetView();
  Refresh();
}

void PlotView::ClearPlot() {
  delete data_.root;
  data_.root = nullptr;
  hasFunction_ = false;
  ResetView();
  Refresh();
}

void PlotView::ZoomIn() {
  zoomFactor_ *= 1.25;
  Refresh();
}

void PlotView::ZoomOut() {
  zoomFactor_ /= 1.25;
  Refresh();
}

void PlotView::ResetView() {
  zoomFactor_ = 1.0;
  panX_ = 0.0;
  panY_ = 0.0;
  Refresh();
}

void PlotView::OnMouseDown(wxMouseEvent &event) {
  if (!hasFunction_)
    return;
  dragging_ = true;
  dragStartPixel_ = event.GetPosition();
  dragStartPanX_ = panX_;
  dragStartPanY_ = panY_;
  CaptureMouse();
  SetCursor(wxCursor(wxCURSOR_SIZING));
}

void PlotView::OnMouseUp(wxMouseEvent & /*event*/) {
  if (!dragging_)
    return;
  dragging_ = false;
  if (HasCapture())
    ReleaseMouse();
  SetCursor(wxNullCursor);
}

void PlotView::OnMouseMove(wxMouseEvent &event) {
  if (!dragging_)
    return;
  wxPoint cur = event.GetPosition();
  double dx = cur.x - dragStartPixel_.x;
  double dy = cur.y - dragStartPixel_.y;
  // Convert pixel delta to data-space pan. Y is inverted in screen coords, so
  // dragging down (positive dy) shifts the view to higher data-y values.
  panX_ = dragStartPanX_ - dx / pixelsPerUnitX_;
  panY_ = dragStartPanY_ + dy / pixelsPerUnitY_;
  Refresh();
}

void PlotView::OnMouseLeave(wxMouseEvent & /*event*/) {
  // If the user drags out of the panel and releases there, OnMouseUp may not
  // fire here. Releasing capture in that case keeps the state consistent.
  if (dragging_ && !HasCapture()) {
    dragging_ = false;
    SetCursor(wxNullCursor);
  }
}

void PlotView::SetNumBars(int n) {
  data_.numBars = std::max(1, n);
  Refresh();
}

int PlotView::ToScreenX(double x, const PlotViewLayout &view, const PlotRange &range) const {
  int offset = (x - range.xMin) / (range.xMax - range.xMin) * (view.w - 2 * view.margin);
  return view.margin + offset;
}

int PlotView::ToScreenY(double y, const PlotViewLayout &view, const PlotRange &range) const {
  int offset = (y - range.yMin) / (range.yMax - range.yMin) * (view.h - 2 * view.margin);
  return view.h - view.margin - offset;
}

// Paint
void PlotView::OnPaint(wxPaintEvent & /*event*/) {
  wxAutoBufferedPaintDC pdc(this);
  wxGCDC dc(pdc);

  wxColour backgroundColor = wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW);
  dc.SetBackground(wxBrush(backgroundColor));
  dc.Clear();

  int panelWidth = 0;
  int panelHeight = 0;
  GetClientSize(&panelWidth, &panelHeight);
  int plotMargin = FromDIP(50);

  const wxColour textColor = wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT);

  if (!hasFunction_ || !data_.root) {
    DrawEmptyPlotPlaceholder(dc, panelWidth, panelHeight, textColor);
    return;
  }

  auto [xMin, xMax] = ComputeXRange(data_);
  auto [yRawMin, yRawMax] = ComputeRobustYBounds(CollectFiniteYValues(data_.root, xMin, xMax));
  auto [yMin, yMax] = AddVisibilityPadding(yRawMin, yRawMax);

  int drawableWidth  = panelWidth  - 2 * plotMargin;
  int drawableHeight = panelHeight - 2 * plotMargin;
  EnforceUnitScaleOneToOne(xMin, xMax, yMin, yMax, drawableWidth, drawableHeight);

  // Apply user zoom & pan on top of the auto-fit range.
  double xCenter = (xMin + xMax) / 2.0;
  double yCenter = (yMin + yMax) / 2.0;
  double xSpan   = (xMax - xMin) / zoomFactor_;
  double ySpan   = (yMax - yMin) / zoomFactor_;
  xMin = xCenter + panX_ - xSpan / 2.0;
  xMax = xCenter + panX_ + xSpan / 2.0;
  yMin = yCenter + panY_ - ySpan / 2.0;
  yMax = yCenter + panY_ + ySpan / 2.0;

  // Cache pixels-per-unit so the drag handler can convert pixel deltas into
  // data-space pan adjustments.
  pixelsPerUnitX_ = drawableWidth  / xSpan;
  pixelsPerUnitY_ = drawableHeight / ySpan;

  const PlotViewLayout view     = {panelWidth, panelHeight, plotMargin};
  const PlotRange      range    = {xMin, xMax, yMin, yMax};
  const BaseColors     baseCols = {wxSystemSettings::GetColour(wxSYS_COLOUR_GRAYTEXT),
                                   textColor, textColor};
  const DataColors     dataCols = BuildDataColors(textColor);

  DrawAxes(dc, view, range, baseCols);

  dc.SetClippingRegion(plotMargin, plotMargin,
                       panelWidth - 2 * plotMargin, panelHeight - 2 * plotMargin);
  DrawBars(dc, view, range, dataCols.bar);
  DrawFunction(dc, view, range, dataCols);
  dc.DestroyClippingRegion();
  DrawBoundLabels(dc, view, range, dataCols.bound);

  DrawPlotTitleAndSubtitle(dc, panelWidth, data_, textColor, textColor);
}

void PlotView::OnResize(wxSizeEvent & /*event*/) {
  Refresh();
}
