#ifndef GUI_VIEW_MAIN_MENU_BAR_H
#define GUI_VIEW_MAIN_MENU_BAR_H

#include <wx/menu.h>

// Menu bar for the main window. Holds only the structure (which menus, which
// items, which accelerators); the actions are wired up by MainWindow via the
// IDs exposed below.
class MainMenuBar : public wxMenuBar {
public:
  // Stable IDs the parent frame binds against. wxID_EXIT is reused for Quit
  // because macOS picks it up automatically and routes it to the app menu.
  static constexpr int kQuit = wxID_EXIT;
  static constexpr int kHelp = wxID_HELP;
  static constexpr int kHistory       = wxID_HIGHEST + 100;
  static constexpr int kImportCsv    = wxID_HIGHEST + 101;
  static constexpr int kExportCsv    = wxID_HIGHEST + 102;
  static constexpr int kLogOut       = wxID_HIGHEST + 103;
  static constexpr int kClearHistory  = wxID_HIGHEST + 104;
  static constexpr int kSearchHistory = wxID_HIGHEST + 105;
  static constexpr int kZoomIn        = wxID_HIGHEST + 106;
  static constexpr int kZoomOut       = wxID_HIGHEST + 107;
  static constexpr int kResetView     = wxID_HIGHEST + 108;

  MainMenuBar();
};

#endif
