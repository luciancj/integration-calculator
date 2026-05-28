#ifndef GUI_DIALOG_HISTORY_H
#define GUI_DIALOG_HISTORY_H

#include "../auth/history.h"
#include <wx/wx.h>

class HistoryDialog : public wxDialog {
public:
  HistoryDialog(wxWindow *parent, const wxString &username, gui::auth::History &history,
                bool focusSearch = false);
};

#endif
