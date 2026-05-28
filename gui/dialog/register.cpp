#include "register.h"

namespace gui::auth {

RegisterDialog::RegisterDialog(wxWindow *parent, UserStore &store)
    : AuthDialog(parent, "Create Account", store) {
  buildLayout("Create Account", wxEmptyString);
}

bool RegisterDialog::onSubmit() {
  std::string user, pass;
  if (!readAndValidate(user, pass))
    return false;
  if (!store_.registerUser(user, pass)) {
    showError("That username is already taken.");
    return false;
  }
  resultUsername_ = user;
  return true;
}

} // namespace gui::auth
