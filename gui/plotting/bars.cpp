// Riemann bars: one rectangle per segment, midpoint-evaluated, drawn from
// the curve down (or up) to the y=0 axis.

#include "helpers.h"
#include "view.h"
#include <algorithm>
#include <cmath>

class BarRect {
public:
  int x, y, width, height;
};

static BarRect CalculateBarRectangle(const ScreenMapper &mapper, int barIndex, const PlotData &data,
                                     int zeroY) {
  double dx = (data.b - data.a) / data.numBars;
  double xStart = data.a + barIndex * dx;
  double xEnd = data.a + (barIndex + 1) * dx;
  double xMid = (xStart + xEnd) / 2.0;
  double y = data.root->evaluate(xMid);

  if (!std::isfinite(y)) {
    return {0, 0, 0, 0};
  }

  int sx1 = mapper.X(xStart);
  int sx2 = mapper.X(xEnd);
  int sy = mapper.Y(y);

  int rectX = std::min(sx1, sx2);
  int rectW = std::abs(sx2 - sx1);
  int rectY = std::min(sy, zeroY);
  int rectH = std::abs(sy - zeroY);

  return {rectX, rectY, rectW, rectH};
}

void PlotView::DrawBars(wxDC &dc, const PlotViewLayout &view, const PlotRange &range,
                        const wxColour &barCol) {
  if (data_.numBars <= 0)
    return;

  ScreenMapper mapper{this, view, range};
  int zeroY = mapper.Y(0.0);

  dc.SetBrush(wxBrush(barCol));

  // Thinner outline as bar count rises; drop the outline entirely past the
  // threshold so dense bars don't merge into a solid block.
  if (data_.numBars < PlotConstants::kMinBarPenThreshold) {
    wxColour darkCol = barCol.ChangeLightness(85);
    dc.SetPen(wxPen(darkCol, 1));
  } else {
    dc.SetPen(*wxTRANSPARENT_PEN);
  }

  for (int i = 0; i < data_.numBars; ++i) {
    BarRect rect = CalculateBarRectangle(mapper, i, data_, zeroY);

    if (rect.width > 0 && rect.height > 0) {
      dc.DrawRectangle(rect.x, rect.y, rect.width, rect.height);
    }
  }
}
