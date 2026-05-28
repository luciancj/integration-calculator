#include "menu-bar.h"

MainMenuBar::MainMenuBar() {
  auto *fileMenu = new wxMenu();
  fileMenu->Append(kImportCsv, "Import CSV\tCtrl+I");
  fileMenu->Append(kExportCsv, "Export CSV\tCtrl+E");
  fileMenu->AppendSeparator();
  fileMenu->Append(kLogOut, "Log Out");
  fileMenu->AppendSeparator();
  fileMenu->Append(kQuit, "Quit\tCtrl+Q");
  Append(fileMenu, "&File");

  auto *viewMenu = new wxMenu();
  viewMenu->Append(kZoomIn,    "Zoom In\t=");
  viewMenu->Append(kZoomOut,   "Zoom Out\t-");
  viewMenu->Append(kResetView, "Reset View\t0");
  Append(viewMenu, "&View");

  auto *historyMenu = new wxMenu();
  historyMenu->Append(kHistory, "Show All\tCtrl+H");
  historyMenu->Append(kSearchHistory, "Search\tCtrl+F");
  historyMenu->AppendSeparator();
  historyMenu->Append(kClearHistory, "Clear History");
  Append(historyMenu, "Histor&y");

  auto *helpMenu = new wxMenu();
  helpMenu->Append(kHelp, "Supported Syntax\tF1");
  Append(helpMenu, "&Help");
}
