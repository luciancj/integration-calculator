#include "login.h"

#include "register.h"

namespace gui::auth {

LoginDialog::LoginDialog(wxWindow *parent, UserStore &store)
    : AuthDialog(parent, "Log In", store) {
  buildLayout("Log In", "Register");
}

bool LoginDialog::onSubmit() {
  std::string user, pass;
  if (!readAndValidate(user, pass))
    return false;
  if (!store_.verify(user, pass)) {
    showError("Invalid username or password.");
    return false;
  }
  resultUsername_ = user;
  return true;
}

void LoginDialog::onAlt() {
  RegisterDialog dlg(this, store_);
  if (dlg.ShowModal() == wxID_OK) {
    // Registration succeeded — log in as that user immediately.
    resultUsername_ = dlg.resultUsername();
    EndModal(wxID_OK);
  }
}

} // namespace gui::auth
