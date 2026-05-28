// MainFrame event handlers
// UI construction (constructor) lives in main_frame_ui.cpp.

#include "window.h"
#include "../../engine/csv/processor.h"
#include "../../engine/expression/exception.h"
#include "../../engine/expression/parser.h"
#include "../../engine/integration/integrator.h"
#include "../../engine/integration/stats.h"
#include "../content/window-strings.h"
#include "../dialog/history.h"
#include <fstream>
#include <wx/filedlg.h>
#include <wx/numformatter.h>

namespace {
wxString FormatFixedSixDecimals(double value) {
  return wxNumberFormatter::ToString(value, 6);
}
} // namespace

using engine::calculateStats;
using engine::IntegrationStats;
using engine::Node;

namespace strings = gui::content;

int MainWindow::SegmentsFromSliderPosition(int sliderPos) const {
  if (sliderPos <= 0) {
    return 1;
  }

  return sliderPos * 10;
}

int MainWindow::SliderPositionFromSegments(int segments) const {
  if (segments <= 1) {
    return 0;
  }

  return segments / 10;
}

// Evaluates a bound field. Accepts plain numbers and any constant expression
// supported by the parser (e.g. "pi", "-pi/2", "2*e"). Plain-number fast path
// avoids parser overhead for the common case. On failure the error side carries
// the reason, which the caller shows to the user.
static std::expected<double, std::string> ParseBound(const wxString &input) {
  wxString trimmed = input;
  trimmed.Trim(true).Trim(false);
  if (trimmed.IsEmpty())
    return std::unexpected(strings::ERR_BOUND_EMPTY);
  double fast = 0.0;
  if (trimmed.ToDouble(&fast))
    return fast;
  // Not a plain number, so let the engine try it as a constant expression.
  return engine::evaluateConstant(std::string(trimmed.mb_str()));
}

bool MainWindow::TryReadAndValidateInput(std::string &functionExpr, double &a, double &b,
                                         int &segments, int &methodSelection) {
  wxString trimmedInput = functionInput_->GetValue().Trim().Trim(false);
  if (trimmedInput.IsEmpty()) {
    ShowError(strings::ERR_EMPTY_FUNCTION);
    return false;
  }

  auto lowerBound = ParseBound(lowerLimitInput_->GetValue());
  auto upperBound = ParseBound(upperLimitInput_->GetValue());
  if (!lowerBound || !upperBound) {
    // Show the reason for whichever bound failed first.
    const std::string &reason = !lowerBound ? lowerBound.error() : upperBound.error();
    ShowError(wxString::FromUTF8(strings::ERR_BOUND_INVALID(reason)));
    return false;
  }
  a = *lowerBound;
  b = *upperBound;

  if (a >= b) {
    ShowError(strings::ERR_BOUND_ORDER);
    return false;
  }

  functionExpr = std::string(trimmedInput.mb_str());
  segments = SegmentsFromSliderPosition(precisionSlider_->GetValue());
  methodSelection = methodChoice_->GetSelection();
  return true;
}

Node *MainWindow::TryParseExpressionTree(const std::string &functionExpr) {
  // The engine throws ParseException on bad syntax; surface its message so the
  // user sees the specific reason. Parse errors used to be printed to stderr,
  // which is invisible inside an .app bundle.
  try {
    return engine::parseExpression(functionExpr);
  } catch (const engine::ParseException &e) {
    ShowError(wxString::FromUTF8(e.what()), strings::ERR_TITLE_PARSE);
    return nullptr;
  }
}

// The method dropdown lists methods in this order, so the selection index
// maps directly to the enum.
static engine::IntegrationMethod methodFromDropdownIndex(int index) {
  switch (index) {
  case 0:
    return engine::IntegrationMethod::RightRiemann;
  case 1:
    return engine::IntegrationMethod::LeftRiemann;
  case 2:
    return engine::IntegrationMethod::MidpointRiemann;
  case 3:
    return engine::IntegrationMethod::Trapezoidal;
  default:
    return engine::IntegrationMethod::Simpsons;
  }
}

double MainWindow::ApplyIntegrationMethod(const Node *root, double a, double b, int segments,
                                          int methodSelection) const {
  return engine::integrateFixed(methodFromDropdownIndex(methodSelection), root, a, b, segments);
}

void MainWindow::UpdateResultLabels(const IntegrationStats &stats) {
  areaLabel_->SetLabel(FormatFixedSixDecimals(stats.getArea()));
  minLabel_->SetLabel(FormatFixedSixDecimals(stats.getMinVal()));
  maxLabel_->SetLabel(FormatFixedSixDecimals(stats.getMaxVal()));
  meanLabel_->SetLabel(FormatFixedSixDecimals(stats.getMeanVal()));
  rmsLabel_->SetLabel(FormatFixedSixDecimals(stats.getRmsVal()));
  deltaLabel_->SetLabel(FormatFixedSixDecimals(stats.getDelta()));
  segmentsLabel_->SetLabel(wxString::Format("%d", stats.getSegmentCount()));
}

void MainWindow::ResetResultLabels() {
  wxString dash = wxString::FromUTF8("\u2014");
  areaLabel_->SetLabel(dash);
  minLabel_->SetLabel(dash);
  maxLabel_->SetLabel(dash);
  meanLabel_->SetLabel(dash);
  rmsLabel_->SetLabel(dash);
  deltaLabel_->SetLabel(dash);
  segmentsLabel_->SetLabel(dash);
}

// Calculate
void MainWindow::OnCalculate(wxCommandEvent & /*event*/) {
  std::string functionExpr;
  double a = 0.0;
  double b = 0.0;
  int segments = 0;
  int methodSelection = -1;
  bool isInputValid = TryReadAndValidateInput(functionExpr, a, b, segments, methodSelection);
  if (!isInputValid) {
    return;
  }

  Node *root = TryParseExpressionTree(functionExpr);
  if (!root) {
    return;
  }

  // Update the parsed-expression label with the AST's own toString. This shows
  // the user *what the parser actually understood* — useful when their input
  // is unusual (e.g. "sin x" vs. "sin(x)") or implicitly bracketed.
  parsedExprLabel_->SetLabel(wxString::FromUTF8(root->toString().c_str()));

  // Integration evaluates the function at many sample points; an unknown
  // function name surfaces here as an EvaluationException. Report it and stop:
  // there is no result to show, and a broken tree must not reach the plot.
  IntegrationStats stats;
  try {
    double result = ApplyIntegrationMethod(root, a, b, segments, methodSelection);
    stats = calculateStats(root, a, b, segments, result);
  } catch (const engine::EvaluationException &e) {
    ShowError(wxString::FromUTF8(e.what()), strings::kTitleEvaluationWarning);
    delete root;
    return;
  }

  // A non-finite result means the function blew up in the interval (e.g. a
  // singularity). Show a clear message and a clean "undefined" state rather
  // than inf/nan in the labels, and skip the plot and history.
  if (!stats.isFinite()) {
    ShowError(wxString::FromUTF8(strings::ERR_NONFINITE_RESULT), strings::kTitleEvaluationWarning);
    ResetResultLabels();
    plotView_->ClearPlot();
    delete root;
    return;
  }

  UpdateResultLabels(stats);

  plotView_->SetNumBars(segments);
  plotView_->SetFunction(root->clone(), a, b);
  delete root;

  gui::auth::HistoryEntry entry;
  entry.function = functionExpr;
  entry.lower = a;
  entry.upper = b;
  entry.segments = segments;
  entry.method = engine::methodName(methodFromDropdownIndex(methodSelection));
  entry.area = stats.getArea();
  history_.append(entry);

  GetSizer()->Layout();
}

void MainWindow::OnShowHistory(wxCommandEvent & /*event*/) {
  HistoryDialog dlg(this, wxString::FromUTF8(username_.c_str()), history_);
  dlg.ShowModal();
}

void MainWindow::OnClearHistory(wxCommandEvent & /*event*/) {
  int answer = wxMessageBox("Delete all history entries? This cannot be undone.", "Clear History",
                            wxYES_NO | wxICON_QUESTION, this);
  if (answer != wxYES)
    return;
  history_.clear();
}

void MainWindow::OnSearchHistory(wxCommandEvent & /*event*/) {
  HistoryDialog dlg(this, wxString::FromUTF8(username_.c_str()), history_, true);
  dlg.ShowModal();
}

// Clear
void MainWindow::OnClear(wxCommandEvent & /*event*/) {
  functionInput_->SetValue(strings::kDefaultFunction);
  lowerLimitInput_->SetValue(strings::kDefaultLowerBound);
  upperLimitInput_->SetValue(strings::kDefaultUpperBound);
  int defaultSegments = 20;
  precisionSlider_->SetValue(SliderPositionFromSegments(defaultSegments));
  precisionValLabel_->SetLabel(wxString::Format("%d", defaultSegments));
  methodChoice_->SetSelection(4);
  parsedExprLabel_->SetLabel(wxString::FromUTF8(strings::SUCCESS_PARSED_AS));

  ResetResultLabels();

  plotView_->ClearPlot();
  GetSizer()->Layout();
}

void MainWindow::ShowError(const wxString &msg, const wxString &title) {
  wxMessageBox(msg, title, wxOK | wxICON_ERROR, this);
}

void MainWindow::OnLogOut(wxCommandEvent & /*event*/) {
  int answer = wxMessageBox("Log out and return to the login screen?", "Log Out",
                            wxYES_NO | wxICON_QUESTION, this);
  if (answer != wxYES)
    return;
  store_.clearSession();
  Close(true);
}

void MainWindow::OnImportCsv(wxCommandEvent & /*event*/) {
  wxFileDialog dlg(this, "Open CSV file", "", "", "CSV files (*.csv)|*.csv",
                   wxFD_OPEN | wxFD_FILE_MUST_EXIST);
  if (dlg.ShowModal() == wxID_CANCEL)
    return;

  std::ifstream in(dlg.GetPath().ToStdString());
  if (!in.is_open()) {
    ShowError("Could not open file.");
    return;
  }

  wxFileDialog saveDlg(this, "", "", "output.csv", "CSV files (*.csv)|*.csv",
                       wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
  if (saveDlg.ShowModal() == wxID_CANCEL)
    return;

  std::ofstream out(saveDlg.GetPath().ToStdString());
  if (!out.is_open()) {
    ShowError("Could not create output file.");
    return;
  }

  int methodIndex = methodChoice_->GetSelection();
  engine::IntegrationMethod method = methodFromDropdownIndex(methodIndex);
  engine::CSVResult result = engine::processCsv(in, out, method);

  for (const auto &row : result.rows) {
    gui::auth::HistoryEntry entry;
    entry.function = row.function;
    entry.lower = row.a;
    entry.upper = row.b;
    entry.segments = row.segments;
    entry.method = engine::methodName(method);
    entry.area = row.area;
    history_.append(entry);
  }

  wxString msg = wxString::Format("Processed %d function(s).", result.processed);
  if (!result.errors.empty())
    msg += wxString::Format("\n%d row(s) had errors and were skipped.", (int)result.errors.size());
  wxMessageBox(msg, "Import Complete", wxOK | wxICON_INFORMATION, this);
}

void MainWindow::OnExportCsv(wxCommandEvent & /*event*/) {
  wxFileDialog saveDlg(this, "", "", "history.csv", "CSV files (*.csv)|*.csv",
                       wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
  if (saveDlg.ShowModal() == wxID_CANCEL)
    return;

  std::ofstream out(saveDlg.GetPath().ToStdString());
  if (!out.is_open()) {
    ShowError("Could not create output file.");
    return;
  }

  out << "Function,Interval Start,Interval End,Segments,Method,Area\n";
  for (const auto &entry : history_.readAll()) {
    out << entry.function << "," << entry.lower << "," << entry.upper << "," << entry.segments
        << "," << entry.method << "," << entry.area << "\n";
  }

  wxMessageBox("History exported successfully.", "Export Complete", wxOK | wxICON_INFORMATION,
               this);
}
