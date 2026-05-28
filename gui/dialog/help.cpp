#include "help.h"

namespace {

struct Row {
  const char *symbol;
  const char *meaning;
};

constexpr Row kOperators[] = {
    {"+  -  *  /", "Add, subtract, multiply, divide"},
    {"^",          "Exponent (right-associative): 2^3^2 = 2^(3^2)"},
    {"-x",         "Unary minus"},
    {"( )",        "Grouping"},
};

constexpr Row kFunctions[] = {
    {"sin(x), cos(x), tan(x)", "Trigonometric (radians)"},
    {"exp(x)",                 "e raised to the x"},
    {"ln(x), log(x)",          "Natural logarithm (positive x only)"},
    {"sqrt(x)",                "Square root (non-negative x only)"},
    {"abs(x)",                 "Absolute value"},
    {"pow(a, b)",              "a raised to the b — note the comma"},
};

constexpr Row kConstants[] = {
    {"pi, PI", "3.14159…"},
    {"e",      "2.71828…"},
};

void AddSection(wxSizer *parent, wxWindow *owner, const wxString &title, const Row *rows,
                std::size_t count, const wxFont &headerFont) {
  auto *header = new wxStaticText(owner, wxID_ANY, title);
  header->SetFont(headerFont);
  parent->Add(header, 0, wxBOTTOM, owner->FromDIP(4));

  auto *grid = new wxFlexGridSizer(2, owner->FromDIP(20), owner->FromDIP(6));
  grid->AddGrowableCol(1, 1);
  for (std::size_t i = 0; i < count; ++i) {
    auto *sym = new wxStaticText(owner, wxID_ANY, rows[i].symbol);
    wxFont mono = sym->GetFont();
    mono.SetFamily(wxFONTFAMILY_TELETYPE);
    sym->SetFont(mono);
    grid->Add(sym, 0);
    grid->Add(new wxStaticText(owner, wxID_ANY, rows[i].meaning), 0, wxEXPAND);
  }
  parent->Add(grid, 0, wxEXPAND | wxBOTTOM, owner->FromDIP(12));
}

} // namespace

HelpDialog::HelpDialog(wxWindow *parent)
    : wxDialog(parent, wxID_ANY, "Supported Syntax", wxDefaultPosition, wxDefaultSize,
               wxDEFAULT_DIALOG_STYLE) {
  SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW));

  wxFont header = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
  header.SetWeight(wxFONTWEIGHT_BOLD);

  auto *intro = new wxStaticText(
      this, wxID_ANY,
      "Enter a function of x using standard math notation. Examples: sin(x)+x, exp(-x*x), pow(x, 3)");
  intro->Wrap(FromDIP(420));

  auto *top = new wxBoxSizer(wxVERTICAL);
  top->Add(intro, 0, wxALL, FromDIP(15));

  auto *body = new wxBoxSizer(wxVERTICAL);
  AddSection(body, this, "Operators", kOperators, std::size(kOperators), header);
  AddSection(body, this, "Functions", kFunctions, std::size(kFunctions), header);
  AddSection(body, this, "Constants", kConstants, std::size(kConstants), header);
  top->Add(body, 0, wxEXPAND | wxLEFT | wxRIGHT, FromDIP(15));

  if (auto *btnSizer = CreateButtonSizer(wxOK))
    top->Add(btnSizer, 0, wxEXPAND | wxALL, FromDIP(10));

  SetSizerAndFit(top);
  CenterOnParent();
}
