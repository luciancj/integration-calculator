#ifndef GUI_DIALOG_LOGIN_H
#define GUI_DIALOG_LOGIN_H

#include "auth.h"

namespace gui::auth {

class LoginDialog : public AuthDialog {
public:
  LoginDialog(wxWindow *parent, UserStore &store);

protected:
  bool onSubmit() override;
  void onAlt() override; // opens RegisterDialog
};

} // namespace gui::auth

#endif
