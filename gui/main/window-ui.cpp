// MainWindow UI Construction
// Builds all widgets, sizers and bindings for the main window.
// Event handlers live in main-window.cpp.

#include "../../engine/integration/integrator.h"
#include "../auth/user-store.h"
#include "../content/window-strings.h"
#include "../dialog/help.h"
#include "../ui/widgets.h"
#include "menu-bar.h"
#include "window.h"
#include <wx/filename.h>
#include <wx/settings.h>
#include <wx/stdpaths.h>

namespace strings = gui::content;
namespace ui = gui::ui;

// ══════════════════════════════════════════════════════════
//  UIStyle — only spacings now; widget styling lives in gui/ui/widgets.cpp
// ══════════════════════════════════════════════════════════

MainWindow::UIStyle MainWindow::UIStyle::from(wxWindow *w) {
  UIStyle s;
  s.sm = w->FromDIP(5);
  s.md = w->FromDIP(10);
  s.lg = w->FromDIP(15);

  s.base = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
  s.base.SetPointSize(s.base.GetPointSize() + 1);

  s.value = s.base;
  s.value.SetWeight(wxFONTWEIGHT_BOLD);
  return s;
}

// ══════════════════════════════════════════════════════════
//  Constructor — high-level layout only
// ══════════════════════════════════════════════════════════

MainWindow::MainWindow(const wxString &title, std::string username,
                       std::filesystem::path historyFilePath, gui::auth::UserStore &store)
    : wxFrame(nullptr, wxID_ANY, title, wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE),
      username_(std::move(username)), history_(std::move(historyFilePath)), store_(store) {
  const auto s = UIStyle::from(this);
  SetIcon(LoadAppIcon());

  SetMenuBar(new MainMenuBar());
  Bind(wxEVT_MENU, [this](wxCommandEvent &) { Close(true); }, MainMenuBar::kQuit);
  Bind(wxEVT_MENU, &MainWindow::OnShowHistory, this, MainMenuBar::kHistory);
  Bind(wxEVT_MENU, &MainWindow::OnClearHistory, this, MainMenuBar::kClearHistory);
  Bind(wxEVT_MENU, &MainWindow::OnSearchHistory, this, MainMenuBar::kSearchHistory);
  Bind(wxEVT_MENU, &MainWindow::OnImportCsv, this, MainMenuBar::kImportCsv);
  Bind(wxEVT_MENU, &MainWindow::OnExportCsv, this, MainMenuBar::kExportCsv);
  Bind(wxEVT_MENU, &MainWindow::OnLogOut, this, MainMenuBar::kLogOut);
  Bind(wxEVT_MENU, [this](wxCommandEvent &) { plotView_->ZoomIn(); }, MainMenuBar::kZoomIn);
  Bind(wxEVT_MENU, [this](wxCommandEvent &) { plotView_->ZoomOut(); }, MainMenuBar::kZoomOut);
  Bind(wxEVT_MENU, [this](wxCommandEvent &) { plotView_->ResetView(); }, MainMenuBar::kResetView);
  Bind(
      wxEVT_MENU,
      [this](wxCommandEvent &) {
        HelpDialog dlg(this);
        dlg.ShowModal();
      },
      MainMenuBar::kHelp);

  plotView_ = new PlotView(this);

  auto *leftSizer = new wxBoxSizer(wxVERTICAL);
  leftSizer->Add(BuildInputSection(s), 0, wxEXPAND | wxALL, s.md);
  leftSizer->Add(BuildButtonRow(s), 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, s.lg);
  leftSizer->Add(BuildResultsPanel(s), 0, wxEXPAND | wxALL, s.md);

  // The plot is a blank canvas — give it an explicit min so Fit() has something
  // to allocate to it; everything else (left column, height) sizes to content.
  plotView_->SetMinSize(FromDIP(wxSize(560, 480)));

  auto *rootSizer = new wxBoxSizer(wxHORIZONTAL);
  leftSizer->SetMinSize(FromDIP(300), -1);
  rootSizer->Add(leftSizer, 0, wxEXPAND | wxFIXED_MINSIZE);
  rootSizer->Add(plotView_, 1, wxEXPAND | wxALL, s.lg);

  SetSizer(rootSizer);
  // Fit to the natural size of the content, lock that as the minimum, then
  // open the window a bit roomier so the plot has space to breathe.
  Fit();
  SetSizeHints(GetSize());
  SetSize(FromDIP(wxSize(1100, 720)));
}

// ══════════════════════════════════════════════════════════
//  Section builders
// ══════════════════════════════════════════════════════════

wxIcon MainWindow::LoadAppIcon() {
  bool dark = wxSystemSettings::GetAppearance().IsDark();
  wxString name =
      dark ? "Integral-iOS-Dark-1024x1024@1x.png" : "Integral-iOS-Default-1024x1024@1x.png";

  // On macOS the bundle ResourcesDir is correct; on Linux the executable
  // lives in build/ and the assets tree is one level up.
  auto tryLoad = [&](const wxString &dir) -> wxIcon {
    wxFileName p(dir, name);
    wxBitmap bmp(p.GetFullPath(), wxBITMAP_TYPE_PNG);
    wxIcon icon;
    if (bmp.IsOk())
      icon.CopyFromBitmap(bmp);
    return icon;
  };

  wxIcon icon = tryLoad(wxStandardPaths::Get().GetResourcesDir());
  if (icon.IsOk())
    return icon;

  // Fallback: look for assets/icons/ relative to the executable.
  wxFileName exeDir(wxStandardPaths::Get().GetExecutablePath());
  wxString assetsDir = exeDir.GetPath() + "/../assets/icons";
  return tryLoad(assetsDir);
}

wxSizer *MainWindow::BuildInputSection(const UIStyle &s) {
  auto *box = new wxBoxSizer(wxVERTICAL);

  // Function field with tooltip
  functionInput_ = ui::makeInputField(this, strings::kDefaultFunction, /*minWidthDip*/ 80);
  functionInput_->SetToolTip(strings::kFunctionTooltip);
  box->Add(ui::labeledColumn(this, strings::kFunctionLabel, functionInput_), 0, wxEXPAND | wxALL,
           s.sm);

  // "Parsed as: …" — populated after each successful parse with the AST's own
  // toString. Italic + muted so it reads as ancillary info next to the input.
  parsedExprLabel_ =
      new wxStaticText(this, wxID_ANY, wxString::FromUTF8(strings::SUCCESS_PARSED_AS));
  wxFont parsedFont = parsedExprLabel_->GetFont();
  parsedFont.SetStyle(wxFONTSTYLE_ITALIC);
  parsedExprLabel_->SetFont(parsedFont);
  parsedExprLabel_->SetForegroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_GRAYTEXT));
  box->Add(ui::labeledColumn(this, strings::SUCCESS_PARSED_LABEL, parsedExprLabel_), 0,
           wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, s.sm);

  // Bound fields
  lowerLimitInput_ = ui::makeInputField(this, strings::kDefaultLowerBound, 80);
  box->Add(ui::labeledColumn(this, strings::kLowerBoundLabel, lowerLimitInput_), 0,
           wxEXPAND | wxALL, s.sm);

  upperLimitInput_ = ui::makeInputField(this, strings::kDefaultUpperBound, 80);
  box->Add(ui::labeledColumn(this, strings::kUpperBoundLabel, upperLimitInput_), 0,
           wxEXPAND | wxALL, s.sm);

  // Pressing Enter in any input field triggers Calculate. The Calculate
  // button has SetDefault() so wxEVT_TEXT_ENTER on text fields also fires it,
  // but we wire it explicitly for clarity and to be defensive against future
  // layout changes that might steal default-button status.
  auto enterTriggersCalc = [this](wxCommandEvent &) {
    wxCommandEvent fakeClick(wxEVT_BUTTON, wxID_ANY);
    OnCalculate(fakeClick);
  };
  functionInput_->Bind(wxEVT_TEXT_ENTER, enterTriggersCalc);
  lowerLimitInput_->Bind(wxEVT_TEXT_ENTER, enterTriggersCalc);
  upperLimitInput_->Bind(wxEVT_TEXT_ENTER, enterTriggersCalc);

  // Precision: slider + live value label, in one horizontal row
  precisionSlider_ = ui::makeIntSlider(this, 0, 10, SliderPositionFromSegments(10), 100);
  precisionValLabel_ = new wxStaticText(this, wxID_ANY, "10");
  precisionValLabel_->SetFont(s.value);
  precisionSlider_->Bind(wxEVT_SLIDER, [this](wxCommandEvent &) {
    int val = SegmentsFromSliderPosition(precisionSlider_->GetValue());
    precisionValLabel_->SetLabel(wxString::Format("%d", val));
    plotView_->SetNumBars(val);
  });

  auto *sliderRow = new wxBoxSizer(wxHORIZONTAL);
  sliderRow->Add(precisionSlider_, 1, wxALIGN_CENTER_VERTICAL);
  sliderRow->Add(precisionValLabel_, 0, wxALIGN_CENTER_VERTICAL | wxLEFT, s.sm);
  box->Add(ui::labeledColumn(this, strings::kPrecisionLabel, sliderRow), 0, wxEXPAND | wxALL, s.sm);

  // Method choice — names come from the engine so dropdown order matches the
  // IntegrationMethod enum, which is what main-window.cpp converts back.
  using engine::IntegrationMethod;
  wxArrayString methods;
  methods.Add(wxString::FromUTF8(engine::methodName(IntegrationMethod::RightRiemann)));
  methods.Add(wxString::FromUTF8(engine::methodName(IntegrationMethod::LeftRiemann)));
  methods.Add(wxString::FromUTF8(engine::methodName(IntegrationMethod::MidpointRiemann)));
  methods.Add(wxString::FromUTF8(engine::methodName(IntegrationMethod::Trapezoidal)));
  methods.Add(wxString::FromUTF8(engine::methodName(IntegrationMethod::Simpsons)));
  methodChoice_ = ui::makeChoice(this, methods, /*initial*/ 4, /*minWidthDip*/ 120);
  box->Add(ui::labeledColumn(this, strings::kIntegrationRuleLabel, methodChoice_), 0,
           wxEXPAND | wxALL, s.sm);

  return box;
}

wxSizer *MainWindow::BuildButtonRow(const UIStyle &s) {
  // Two rows: Calculate spans the full column; Clear/History share the row below.
  // Stacking avoids wxBoxSizer's proportion-based min-size inflation, which made
  // the column much wider than the buttons actually needed.
  auto *col = new wxBoxSizer(wxVERTICAL);

  auto *calcBtn = ui::makeButton(this, strings::BUTTON_CALCULATE);
  calcBtn->SetDefault();
  calcBtn->Bind(wxEVT_BUTTON, &MainWindow::OnCalculate, this);
  col->Add(calcBtn, 0, wxEXPAND | wxBOTTOM, s.sm);

  auto *clearBtn = ui::makeButton(this, strings::BUTTON_CLEAR);
  clearBtn->Bind(wxEVT_BUTTON, &MainWindow::OnClear, this);
  col->Add(clearBtn, 0, wxEXPAND | wxBOTTOM, s.sm);

  // Zoom row: buttons forward to PlotView; capture by `this` because the
  // plot pointer outlives the buttons (both owned by MainWindow).
  auto *zoomInBtn =
      ui::makeButton(this, strings::BUTTON_ZOOM_IN, [this]() { plotView_->ZoomIn(); });
  auto *zoomOutBtn =
      ui::makeButton(this, strings::kZoomOutButton, [this]() { plotView_->ZoomOut(); });
  auto *resetBtn =
      ui::makeButton(this, strings::kResetViewButton, [this]() { plotView_->ResetView(); });

  auto *zoomRow = new wxBoxSizer(wxHORIZONTAL);
  zoomRow->Add(zoomInBtn, 1, wxRIGHT, s.sm);
  zoomRow->Add(zoomOutBtn, 1, wxRIGHT, s.sm);
  zoomRow->Add(resetBtn, 1);
  col->Add(zoomRow, 0, wxEXPAND);

  return col;
}

wxSizer *MainWindow::BuildResultsPanel(const UIStyle &s) {
  auto *box = new wxBoxSizer(wxVERTICAL);
  AddResultRow(box, s, wxString::FromUTF8(strings::kResultArea), areaLabel_);
  AddResultRow(box, s, wxString::FromUTF8(strings::kResultMin), minLabel_);
  AddResultRow(box, s, wxString::FromUTF8(strings::kResultMax), maxLabel_);
  AddResultRow(box, s, wxString::FromUTF8(strings::kResultMean), meanLabel_);
  AddResultRow(box, s, wxString::FromUTF8(strings::kResultRms), rmsLabel_);
  AddResultRow(box, s, wxString::FromUTF8(strings::kResultDelta), deltaLabel_);
  AddResultRow(box, s, wxString::FromUTF8(strings::kResultSegments), segmentsLabel_);
  return box;
}

// ══════════════════════════════════════════════════════════
//  Row helpers
// ══════════════════════════════════════════════════════════

void MainWindow::AddResultRow(wxSizer *box, const UIStyle &s, const wxString &name,
                              wxStaticText *&label) {
  // Stack name above the value so a wide number can use the row's full width
  // instead of being squeezed by the name on the same line.
  auto *col = new wxBoxSizer(wxVERTICAL);
  col->Add(ui::makeMutedLabel(this, name), 0, wxBOTTOM, 2);
  label = new wxStaticText(this, wxID_ANY, wxString::FromUTF8("—"));
  label->SetFont(s.value);
  col->Add(label, 0);
  box->Add(col, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, FromDIP(6));
}
