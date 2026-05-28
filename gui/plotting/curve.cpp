// Function curve: samples f(x) across the visible x-range, drawing either
// through a wxGraphicsContext (smooth, antialiased) or a plain wxDC fallback.
// Also draws the dashed integration-bound markers.

#include "helpers.h"
#include "view.h"
#include <cmath>
#include <optional>
#include <wx/dcgraph.h>
#include <wx/graphics.h>

class FunctionSample {
public:
  double x;
  double y;
};

static std::optional<FunctionSample> SampleFunction(const engine::Node *root,
                                                    const PlotRange &range, int i) {
  double t = 1.0 * i / PlotConstants::kDrawSamples;
  double x = range.xMin + (range.xMax - range.xMin) * t;
  double y = root->evaluate(x);
  if (!IsValidAndInRange(y, range))
    return std::nullopt;
  return FunctionSample{x, y};
}

static wxPoint2DDouble ToGraphPoint(const FunctionSample &sample, const PlotViewLayout &view,
                                    const PlotRange &range) {
  double clampedY = ClampToRange(sample.y, range);
  double sx = view.margin +
              (sample.x - range.xMin) / (range.xMax - range.xMin) * (view.w - 2 * view.margin);
  double sy = view.h - view.margin -
              (clampedY - range.yMin) / (range.yMax - range.yMin) * (view.h - 2 * view.margin);
  return wxPoint2DDouble(sx, sy);
}

static wxGraphicsPath BuildFunctionPath(wxGraphicsContext *gc, const engine::Node *root,
                                        const ScreenMapper &mapper) {
  const PlotViewLayout &view = mapper.view;
  const PlotRange &range = mapper.range;
  wxGraphicsPath path = gc->CreatePath();
  bool started = false;

  for (int i = 0; i <= PlotConstants::kDrawSamples; ++i) {
    auto sample = SampleFunction(root, range, i);
    if (!sample) {
      started = false;
      continue;
    }

    wxPoint2DDouble p = ToGraphPoint(*sample, view, range);

    if (!started) {
      path.MoveToPoint(p.m_x, p.m_y);
      started = true;
    } else {
      path.AddLineToPoint(p.m_x, p.m_y);
    }
  }

  return path;
}

static void DrawFunctionWithGraphics(wxGraphicsContext *gc, const engine::Node *root,
                                     const ScreenMapper &mapper, const DataColors &colors) {
  wxGraphicsPath path = BuildFunctionPath(gc, root, mapper);
  gc->SetPen(gc->CreatePen(wxGraphicsPenInfo(colors.curve).Width(2.8)));
  gc->StrokePath(path);
}

static void DrawFunctionFallback(wxDC &dc, const ScreenMapper &mapper, const engine::Node *root,
                                 const DataColors &colors) {
  const PlotRange &range = mapper.range;
  dc.SetPen(wxPen(colors.curve, 3));

  int prevSx = 0, prevSy = 0;
  bool hasPrev = false;

  for (int i = 0; i <= PlotConstants::kDrawSamples; ++i) {
    double t = 1.0 * i / PlotConstants::kDrawSamples;
    double x = range.xMin + (range.xMax - range.xMin) * t;
    double y = root->evaluate(x);

    if (!std::isfinite(y)) {
      hasPrev = false;
      continue;
    }

    int sx = mapper.X(x);
    int sy = mapper.Y(y);

    if (hasPrev) {
      dc.DrawLine(prevSx, prevSy, sx, sy);
    }

    prevSx = sx;
    prevSy = sy;
    hasPrev = true;
  }
}

static void DrawBoundaryMarkers(wxDC &dc, const ScreenMapper &mapper, double a, double b,
                                const wxColour &col) {
  const PlotViewLayout &view = mapper.view;
  dc.SetPen(wxPen(col, 1, wxPENSTYLE_SHORT_DASH));

  int boundaryAX = mapper.X(a);
  int boundaryBX = mapper.X(b);

  dc.DrawLine(boundaryAX, view.margin, boundaryAX, view.h - view.margin);
  dc.DrawLine(boundaryBX, view.margin, boundaryBX, view.h - view.margin);
}

void PlotView::DrawFunction(wxDC &dc, const PlotViewLayout &view, const PlotRange &range,
                            const DataColors &colors) {
  ScreenMapper mapper{this, view, range};
  wxGraphicsContext *gc = dc.GetGraphicsContext();

  if (gc) {
    DrawFunctionWithGraphics(gc, data_.root, mapper, colors);
  } else {
    DrawFunctionFallback(dc, mapper, data_.root, colors);
  }

  DrawBoundaryMarkers(dc, mapper, data_.a, data_.b, colors.bound);
}
