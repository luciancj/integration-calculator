#ifndef GUI_PLOTTING_HELPERS_H
#define GUI_PLOTTING_HELPERS_H

#include "view.h"
#include <cmath>
#include <wx/numformatter.h>
#include <wx/wx.h>

// Shared helpers used by the plot-rendering translation units (axes, bars,
// curve, bounds). Header-only so each .cpp gets its own copy of the inline
// helpers without needing a separate .cpp.

namespace PlotConstants {
constexpr int kDrawSamples = 1000;
constexpr int kMinBarPenThreshold = 100;
constexpr int kGridDivisor = 8;
constexpr double kOutOfRangeMargin = 0.5;
} // namespace PlotConstants

inline bool IsValidAndInRange(double y, const PlotRange &range) {
  if (!std::isfinite(y))
    return false;
  double margin = (range.yMax - range.yMin) * PlotConstants::kOutOfRangeMargin;
  return y >= (range.yMin - margin) && y <= (range.yMax + margin);
}

inline double ClampToRange(double value, const PlotRange &range) {
  if (value < range.yMin) {
    return range.yMin;
  }

  if (value > range.yMax) {
    return range.yMax;
  }

  return value;
}

inline wxString FormatAxisValue(double value) {
  int precision = 0;
  if (std::abs(value) < 100) {
    precision = 1;
  }
  return wxNumberFormatter::ToString(value, precision);
}

class ScreenMapper {
public:
  PlotView *plotView;
  const PlotViewLayout &view;
  const PlotRange &range;

  int X(double x) const {
    return plotView->ToScreenX(x, view, range);
  }

  int Y(double y) const {
    return plotView->ToScreenY(y, view, range);
  }
};

#endif
