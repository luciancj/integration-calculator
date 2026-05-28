// Bound labels: the "a = ..." / "b = ..." text drawn under each integration
// boundary, outside the plot's clipping region.

#include "helpers.h"
#include "view.h"
#include <wx/numformatter.h>

static void DrawBoundaryLabel(wxDC &dc, const std::string &prefix, double value, int screenX,
                              const PlotViewLayout &view) {
  wxString numericValue = wxNumberFormatter::ToString(value, 6);
  wxString label = wxString::Format("%s = %s", prefix, numericValue);
  wxSize labelSize = dc.GetTextExtent(label);

  int labelX = screenX - labelSize.GetWidth() / 2;
  int labelY = view.h - view.margin + (view.margin - labelSize.GetHeight()) / 2;

  dc.DrawText(label, labelX, labelY);
}

void PlotView::DrawBoundLabels(wxDC &dc, const PlotViewLayout &view, const PlotRange &range,
                               const wxColour &col) {
  ScreenMapper mapper{this, view, range};
  int aX = mapper.X(data_.a);
  int bX = mapper.X(data_.b);

  dc.SetFont(wxFont(10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));
  dc.SetTextForeground(col);

  DrawBoundaryLabel(dc, "a", data_.a, aX, view);
  DrawBoundaryLabel(dc, "b", data_.b, bX, view);
}
