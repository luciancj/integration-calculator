#ifndef GUI_VIEW_MAIN_WINDOW_H
#define GUI_VIEW_MAIN_WINDOW_H

#include "../../engine/integration/stats.h"
#include "../auth/history.h"
#include "../auth/user-store.h"
#include "../plotting/view.h"
#include <filesystem>
#include <string>
#include <wx/slider.h>
#include <wx/wx.h>

class MainWindow : public wxFrame {
public:
  MainWindow(const wxString &title, std::string username,
             std::filesystem::path historyFilePath, gui::auth::UserStore &store);

private:
  // Theme
  // Just spacings + a couple of fonts now; widget styling lives in gui/ui.
  struct UIStyle {
    int sm, md, lg;
    wxFont base, value;
    static UIStyle from(wxWindow *w);
  };

  // UI section builders (each returns a sizer or widget)
  wxIcon LoadAppIcon();
  wxSizer *BuildInputSection(const UIStyle &s);
  wxSizer *BuildButtonRow(const UIStyle &s);
  wxSizer *BuildResultsPanel(const UIStyle &s);

  void AddResultRow(wxSizer *box, const UIStyle &s, const wxString &name, wxStaticText *&label);

  // Event handlers
  void OnCalculate(wxCommandEvent &event);
  void OnClear(wxCommandEvent &event);
  void OnShowHistory(wxCommandEvent &event);
  void OnClearHistory(wxCommandEvent &event);
  void OnSearchHistory(wxCommandEvent &event);
  void OnImportCsv(wxCommandEvent &event);
  void OnExportCsv(wxCommandEvent &event);
  void OnLogOut(wxCommandEvent &event);
  void ShowError(const wxString &msg, const wxString &title = "Input Error");

  // Calculation helpers
  int SegmentsFromSliderPosition(int sliderPos) const;
  int SliderPositionFromSegments(int segments) const;
  bool TryReadAndValidateInput(std::string &functionExpr, double &a, double &b, int &segments,
                               int &methodSelection);
  engine::Node *TryParseExpressionTree(const std::string &functionExpr);
  double ApplyIntegrationMethod(const engine::Node *root, double a, double b, int segments,
                                int methodSelection) const;

  // UI update helpers
  void UpdateResultLabels(const engine::IntegrationStats &stats);
  void ResetResultLabels();

  // Input controls
  wxTextCtrl *functionInput_;
  wxTextCtrl *lowerLimitInput_;
  wxTextCtrl *upperLimitInput_;
  wxSlider *precisionSlider_;
  wxStaticText *precisionValLabel_;
  wxChoice *methodChoice_;

  // Shows the AST's toString() after a successful parse — lets the user see
  // exactly what the parser understood (helps diagnose missing operators,
  // implicit bracketing, etc.).
  wxStaticText *parsedExprLabel_;

  // Result labels
  wxStaticText *areaLabel_;
  wxStaticText *minLabel_;
  wxStaticText *maxLabel_;
  wxStaticText *meanLabel_;
  wxStaticText *rmsLabel_;
  wxStaticText *deltaLabel_;
  wxStaticText *segmentsLabel_;

  // Plot
  PlotView *plotView_;

  // Session
  std::string username_;
  gui::auth::History history_;
  gui::auth::UserStore &store_;
};

#endif
