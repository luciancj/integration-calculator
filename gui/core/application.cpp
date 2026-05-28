#include "application.h"
#include "../dialog/login.h"
#include "../auth/user-store.h"
#include "../main/window.h"
#include <filesystem>
#include <wx/settings.h>
#include <wx/stdpaths.h>
#include <wx/sysopt.h>

wxIMPLEMENT_APP_NO_MAIN(IntegrationApp);

namespace {

// Per-user app data dir, ensured to exist. Falls back to current dir.
std::filesystem::path userDataPath(const wxString &fileName) {
  wxString dir = wxStandardPaths::Get().GetUserDataDir();
  std::filesystem::path base(dir.ToStdString());
  std::error_code ec;
  std::filesystem::create_directories(base, ec);
  if (ec)
    base = std::filesystem::current_path();
  return base / fileName.ToStdString();
}

} // namespace

bool IntegrationApp::OnInit() {
  wxInitAllImageHandlers();

  store_ = std::make_unique<gui::auth::UserStore>(userDataPath("users.txt"));

  std::string username = store_->loadSession();
  if (username.empty() || !store_->exists(username)) {
    gui::auth::LoginDialog login(nullptr, *store_);
    if (login.ShowModal() != wxID_OK)
      return false;
    username = login.resultUsername();
    store_->saveSession(username);
  }

  auto historyPath = userDataPath(wxString::Format("history-%s.txt", username));
  auto *frame = new MainWindow("Integration Calculator", username, historyPath, *store_);
  frame->Show(true);
  return true;
}
