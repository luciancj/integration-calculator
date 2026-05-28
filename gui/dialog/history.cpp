#include "history.h"

#include <wx/listctrl.h>
#include <wx/srchctrl.h>

static void populateList(wxListView *list, const std::vector<gui::auth::HistoryEntry> &entries,
                         const wxString &filter) {
  list->DeleteAllItems();
  long row = 0;
  for (auto it = entries.rbegin(); it != entries.rend(); ++it) {
    if (!filter.IsEmpty()) {
      wxString fn = wxString::FromUTF8(it->function);
      if (!fn.Lower().Contains(filter.Lower()))
        continue;
    }
    list->InsertItem(row, it->timestamp);
    list->SetItem(row, 1, wxString::FromUTF8(it->function));
    list->SetItem(row, 2, wxString::Format("%g", it->lower));
    list->SetItem(row, 3, wxString::Format("%g", it->upper));
    list->SetItem(row, 4, wxString::Format("%d", it->segments));
    list->SetItem(row, 5, wxString::FromUTF8(it->method));
    list->SetItem(row, 6, wxString::Format("%.6f", it->area));
    ++row;
  }
  if (row == 0)
    list->InsertItem(0, entries.empty() ? "(no calculations yet)" : "(no results)");
}

HistoryDialog::HistoryDialog(wxWindow *parent, const wxString &username,
                             gui::auth::History &history, bool focusSearch)
    : wxDialog(parent, wxID_ANY, wxString::Format("History — %s", username),
               wxDefaultPosition, FromDIP(wxSize(720, 460)),
               wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER) {
  SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW));

  auto entries = history.readAll();

  auto *list = new wxListView(this, wxID_ANY, wxDefaultPosition, wxDefaultSize,
                              wxLC_REPORT | wxLC_SINGLE_SEL);
  list->AppendColumn("When",     wxLIST_FORMAT_LEFT,  FromDIP(140));
  list->AppendColumn("Function", wxLIST_FORMAT_LEFT,  FromDIP(180));
  list->AppendColumn("a",        wxLIST_FORMAT_RIGHT, FromDIP(60));
  list->AppendColumn("b",        wxLIST_FORMAT_RIGHT, FromDIP(60));
  list->AppendColumn("Segments", wxLIST_FORMAT_RIGHT, FromDIP(70));
  list->AppendColumn("Method",   wxLIST_FORMAT_LEFT,  FromDIP(110));
  list->AppendColumn("Area",     wxLIST_FORMAT_RIGHT, FromDIP(90));

  populateList(list, entries, "");

  const int columnCount = list->GetColumnCount();
  for (int col = 0; col < columnCount; ++col) {
    list->SetColumnWidth(col, wxLIST_AUTOSIZE_USEHEADER);
    int headerWidth = list->GetColumnWidth(col);
    list->SetColumnWidth(col, wxLIST_AUTOSIZE);
    int contentWidth = list->GetColumnWidth(col);
    list->SetColumnWidth(col, std::max(headerWidth, contentWidth) + FromDIP(12));
  }

  auto *searchCtrl = new wxSearchCtrl(this, wxID_ANY, "", wxDefaultPosition,
                                      wxDefaultSize, wxTE_PROCESS_ENTER);
  searchCtrl->ShowSearchButton(true);
  searchCtrl->ShowCancelButton(true);
  searchCtrl->SetHint("Search functions...");

  auto onSearch = [list, entries, searchCtrl](wxCommandEvent &) {
    populateList(list, entries, searchCtrl->GetValue());
  };
  searchCtrl->Bind(wxEVT_TEXT, onSearch);
  searchCtrl->Bind(wxEVT_SEARCHCTRL_CANCEL_BTN, [list, entries, searchCtrl](wxCommandEvent &) {
    searchCtrl->Clear();
    populateList(list, entries, "");
  });

  auto *top = new wxBoxSizer(wxVERTICAL);
  top->Add(searchCtrl, 0, wxEXPAND | wxALL, FromDIP(10));
  top->Add(list, 1, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, FromDIP(10));

  auto *btnRow = new wxBoxSizer(wxHORIZONTAL);
  btnRow->AddStretchSpacer();
  auto *okBtn = new wxButton(this, wxID_OK, "Close");
  okBtn->SetDefault();
  btnRow->Add(okBtn, 0);
  top->Add(btnRow, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, FromDIP(10));

  SetSizer(top);
  SetMinSize(FromDIP(wxSize(640, 300)));
  CenterOnParent();

  if (focusSearch)
    searchCtrl->SetFocus();
}
