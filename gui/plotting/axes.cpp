// Axes & grid: vertical/horizontal grid lines with labels, plus the zero
// axes when they fall inside the visible range.

#include "helpers.h"
#include "view.h"
#include <algorithm>
#include <cmath>

static double CalculateNiceGridStep(double range) {
  double rough = range / PlotConstants::kGridDivisor;
  double mag = std::pow(10.0, std::floor(std::log10(rough)));
  double norm = rough / mag;

  if (norm < 1.5)
    return mag;
  if (norm < 3.0)
    return 2.0 * mag;
  if (norm < 7.0)
    return 5.0 * mag;
  return 10.0 * mag;
}

static void DrawVerticalGridLine(wxDC &dc, double x, int screenX, const PlotViewLayout &view,
                                 const BaseColors &colors) {
  dc.SetPen(wxPen(colors.grid, 1, wxPENSTYLE_SOLID));
  dc.DrawLine(screenX, view.margin, screenX, view.h - view.margin);

  wxString label = FormatAxisValue(x);
  wxSize labelSize = dc.GetTextExtent(label);
  int labelX = screenX - labelSize.GetWidth() / 2;
  int labelY = view.h - view.margin + 6;
  dc.DrawText(label, labelX, labelY);
}

static void DrawHorizontalGridLine(wxDC &dc, double y, int screenY, const PlotViewLayout &view,
                                   const BaseColors &colors) {
  dc.SetPen(wxPen(colors.grid, 1, wxPENSTYLE_SOLID));
  dc.DrawLine(view.margin, screenY, view.w - view.margin, screenY);

  wxString label = FormatAxisValue(y);
  wxSize labelSize = dc.GetTextExtent(label);
  int labelX = view.margin - labelSize.GetWidth() - 8;
  int labelY = screenY - labelSize.GetHeight() / 2;
  dc.DrawText(label, labelX, labelY);
}

static void DrawZeroAxes(wxDC &dc, const ScreenMapper &mapper, const BaseColors &colors) {
  const PlotViewLayout &view = mapper.view;
  const PlotRange &range = mapper.range;
  dc.SetPen(wxPen(colors.axis, 1, wxPENSTYLE_SOLID));

  if (range.yMin <= 0 && range.yMax >= 0) {
    int zeroY = mapper.Y(0);
    dc.DrawLine(view.margin, zeroY, view.w - view.margin, zeroY);
  }

  if (range.xMin <= 0 && range.xMax >= 0) {
    int zeroX = mapper.X(0);
    dc.DrawLine(zeroX, view.margin, zeroX, view.h - view.margin);
  }
}

void PlotView::DrawAxes(wxDC &dc, const PlotViewLayout &view, const PlotRange &range,
                        const BaseColors &colors) {
  dc.SetFont(wxFont(10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));
  dc.SetTextForeground(colors.text);
  ScreenMapper mapper{this, view, range};

  // Unified grid step — uses the smaller axis range so the grid looks
  // isometric (same step in x and y).
  double xRange = range.xMax - range.xMin;
  double yRange = range.yMax - range.yMin;
  double minRange = std::min(xRange, yRange);
  double unifiedStep = CalculateNiceGridStep(minRange);

  double xStart = std::ceil(range.xMin / unifiedStep) * unifiedStep;
  for (double x = xStart; x <= range.xMax; x += unifiedStep) {
    int screenX = mapper.X(x);
    DrawVerticalGridLine(dc, x, screenX, view, colors);
  }

  double yStart = std::ceil(range.yMin / unifiedStep) * unifiedStep;
  for (double y = yStart; y <= range.yMax; y += unifiedStep) {
    int screenY = mapper.Y(y);
    DrawHorizontalGridLine(dc, y, screenY, view, colors);
  }

  DrawZeroAxes(dc, mapper, colors);
}
