
#ifndef GUI_DIALOG_AUTH_H
#define GUI_DIALOG_AUTH_H

#include "../auth/user-store.h"
#include <string>
#include <wx/wx.h>

namespace gui::auth {

// Base class for the login and register dialogs. The two flows share most of
// their UI (username field, password field, action button) and validation
// logic, but differ in what the action button actually does. That difference
// is the abstract onSubmit() hook — this is the template-method pattern.
//
// The shared widgets and helpers are 'protected' (not 'private') because the
// subclasses need to read the field contents and report errors; they are not
// 'public' because nothing outside the hierarchy should touch them.
class AuthDialog : public wxDialog {
public:
  AuthDialog(wxWindow *parent, const wxString &title, UserStore &store);
  ~AuthDialog() override = default;

  // Populated when ShowModal() returns wxID_OK.
  const std::string &resultUsername() const {
    return resultUsername_;
  }

protected:
  // Called by the submit button. Subclass returns true to close the dialog
  // with wxID_OK (and resultUsername_ is then valid).
  virtual bool onSubmit() = 0;

  // Optional secondary action ("Register" / "Back to Login"). Default no-op.
  virtual void onAlt() {}

  // Subclasses call this once during construction to build the shared layout.
  // altButtonLabel may be empty to omit the secondary button.
  void buildLayout(const wxString &submitLabel, const wxString &altButtonLabel);

  // Returns trimmed field values; reports an error and returns false if either
  // is empty.
  bool readAndValidate(std::string &usernameOut, std::string &passwordOut);

  void showError(const wxString &msg);

  UserStore &store_;
  wxTextCtrl *usernameField_ = nullptr;
  wxTextCtrl *passwordField_ = nullptr;
  std::string resultUsername_;

private:
  void OnSubmitClick(wxCommandEvent &);
  void OnAltClick(wxCommandEvent &);
};

} // namespace gui::auth

#endif
