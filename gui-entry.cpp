#include "gui/core/application.h"
#include <wx/wx.h>

int main(int argc, char *argv[]) {
  wxEntryStart(argc, argv);
  wxTheApp->CallOnInit();
  wxTheApp->OnRun();
  wxTheApp->OnExit();
  wxEntryCleanup();
  return 0;
}
