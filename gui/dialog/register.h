#ifndef GUI_DIALOG_REGISTER_H
#define GUI_DIALOG_REGISTER_H

#include "auth.h"

namespace gui::auth {

class RegisterDialog : public AuthDialog {
public:
  RegisterDialog(wxWindow *parent, UserStore &store);

protected:
  bool onSubmit() override;
};

} // namespace gui::auth

#endif
