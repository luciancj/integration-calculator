#include "auth.h"

#include "../ui/widgets.h"

namespace gui::auth {

namespace ui = gui::ui;

namespace {
constexpr int kSubmitId = wxID_OK;
constexpr int kAltId = wxID_HIGHEST + 1;
} // namespace

AuthDialog::AuthDialog(wxWindow *parent, const wxString &title, UserStore &store)
    : wxDialog(parent, wxID_ANY, title, wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE),
      store_(store) {
  SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW));
}

void AuthDialog::buildLayout(const wxString &submitLabel, const wxString &altButtonLabel) {
  const int pad = FromDIP(8);

  auto *topSizer = new wxBoxSizer(wxVERTICAL);

  auto *grid = new wxFlexGridSizer(2, 2, pad, pad);
  grid->AddGrowableCol(1, 1);

  grid->Add(ui::makeLabel(this, "Username:"), 0, wxALIGN_CENTER_VERTICAL);
  usernameField_ = ui::makeInputField(this);
  grid->Add(usernameField_, 1, wxEXPAND);

  grid->Add(ui::makeLabel(this, "Password:"), 0, wxALIGN_CENTER_VERTICAL);
  passwordField_ = ui::makePasswordField(this);
  grid->Add(passwordField_, 1, wxEXPAND);

  // With wxTE_PROCESS_ENTER on, Enter no longer cascades to the default
  // button — bind it explicitly so submit-on-Enter still works.
  auto enterSubmits = [this](wxCommandEvent &) {
    if (onSubmit())
      EndModal(wxID_OK);
  };
  usernameField_->Bind(wxEVT_TEXT_ENTER, enterSubmits);
  passwordField_->Bind(wxEVT_TEXT_ENTER, enterSubmits);

  topSizer->Add(grid, 0, wxEXPAND | wxALL, FromDIP(15));

  auto *btnRow = new wxBoxSizer(wxHORIZONTAL);
  if (!altButtonLabel.IsEmpty()) {
    auto *altBtn = ui::makeButton(this, altButtonLabel, nullptr, kAltId);
    altBtn->Bind(wxEVT_BUTTON, &AuthDialog::OnAltClick, this);
    btnRow->Add(altBtn, 0, wxRIGHT, pad);
  }
  btnRow->AddStretchSpacer();
  // wxID_CANCEL is auto-handled by wxDialog → EndModal(wxID_CANCEL).
  btnRow->Add(ui::makeButton(this, "Cancel", nullptr, wxID_CANCEL), 0, wxRIGHT, pad);
  auto *submitBtn = ui::makeButton(this, submitLabel, nullptr, kSubmitId);
  submitBtn->SetDefault();
  submitBtn->Bind(wxEVT_BUTTON, &AuthDialog::OnSubmitClick, this);
  btnRow->Add(submitBtn, 0);

  topSizer->Add(btnRow, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, FromDIP(15));

  SetSizer(topSizer);
  // Fit to content first, then enforce a wider minimum so the username/password
  // fields have breathing room. Resize the dialog to match.
  topSizer->Fit(this);
  const int wantedWidth = FromDIP(380);
  if (GetSize().GetWidth() < wantedWidth) {
    SetClientSize(wantedWidth, GetClientSize().GetHeight());
  }
  SetMinSize(GetSize());
  CenterOnParent();
  usernameField_->SetFocus();
}

bool AuthDialog::readAndValidate(std::string &usernameOut, std::string &passwordOut) {
  wxString user = usernameField_->GetValue().Trim().Trim(false);
  wxString pass = passwordField_->GetValue();
  if (user.IsEmpty() || pass.IsEmpty()) {
    showError("Username and password are both required.");
    return false;
  }
  usernameOut = std::string(user.mb_str());
  passwordOut = std::string(pass.mb_str());
  return true;
}

void AuthDialog::showError(const wxString &msg) {
  wxMessageBox(msg, "Authentication", wxOK | wxICON_ERROR, this);
}

void AuthDialog::OnSubmitClick(wxCommandEvent &) {
  if (onSubmit())
    EndModal(wxID_OK);
}

void AuthDialog::OnAltClick(wxCommandEvent &) {
  onAlt();
}

} // namespace gui::auth
