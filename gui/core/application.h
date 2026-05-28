#ifndef GUI_CORE_APPLICATION_H
#define GUI_CORE_APPLICATION_H

#include "../auth/user-store.h"
#include <memory>
#include <wx/wx.h>

class IntegrationApp : public wxApp {
public:
  bool OnInit() override;

private:
  std::unique_ptr<gui::auth::UserStore> store_;
};

#endif
