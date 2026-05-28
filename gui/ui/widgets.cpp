#include "widgets.h"

#include <wx/settings.h>

namespace gui::ui {

wxStaticText *makeLabel(wxWindow *parent, const wxString &text) {
  return new wxStaticText(parent, wxID_ANY, text);
}

wxStaticText *makeBoldLabel(wxWindow *parent, const wxString &text) {
  auto *label = new wxStaticText(parent, wxID_ANY, text);
  wxFont f = label->GetFont();
  f.SetWeight(wxFONTWEIGHT_BOLD);
  label->SetFont(f);
  return label;
}

wxStaticText *makeMutedLabel(wxWindow *parent, const wxString &text, int wrapDip) {
  auto *label = new wxStaticText(parent, wxID_ANY, text);
  label->SetForegroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_GRAYTEXT));
  if (wrapDip > 0)
    label->Wrap(parent->FromDIP(wrapDip));
  return label;
}

wxTextCtrl *makeInputField(wxWindow *parent, const wxString &defaultValue, int minWidthDip) {
  // wxTE_PROCESS_ENTER lets the field emit wxEVT_TEXT_ENTER when the user
  // presses Enter, instead of the keystroke being eaten by the parent dialog.
  // Every input field in this app benefits from "Enter submits", so it's on
  // by default — callers that don't bind the event simply ignore it.
  auto *field = new wxTextCtrl(parent, wxID_ANY, defaultValue, wxDefaultPosition, wxDefaultSize,
                               wxTE_PROCESS_ENTER);
  if (minWidthDip > 0)
    field->SetMinSize(parent->FromDIP(wxSize(minWidthDip, -1)));
  return field;
}

wxTextCtrl *makePasswordField(wxWindow *parent) {
  return new wxTextCtrl(parent, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize,
                        wxTE_PASSWORD | wxTE_PROCESS_ENTER);
}

wxButton *makeButton(wxWindow *parent, const wxString &text, std::function<void()> onClick,
                     int id) {
  auto *button = new wxButton(parent, id, text);
  if (onClick)
    button->Bind(wxEVT_BUTTON, [cb = std::move(onClick)](wxCommandEvent &) { cb(); });
  return button;
}

wxSlider *makeIntSlider(wxWindow *parent, int min, int max, int initial, int minWidthDip) {
  auto *slider =
      new wxSlider(parent, wxID_ANY, initial, min, max, wxDefaultPosition, wxDefaultSize,
                   wxSL_HORIZONTAL);
  if (minWidthDip > 0)
    slider->SetMinSize(parent->FromDIP(wxSize(minWidthDip, -1)));
  return slider;
}

wxChoice *makeChoice(wxWindow *parent, const wxArrayString &items, int initialSelection,
                     int minWidthDip) {
  auto *choice = new wxChoice(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, items);
  int itemCount = (int)items.GetCount();
  if (initialSelection >= 0 && initialSelection < itemCount)
    choice->SetSelection(initialSelection);
  if (minWidthDip > 0)
    choice->SetMinSize(parent->FromDIP(wxSize(minWidthDip, -1)));
  return choice;
}

namespace {
wxBoxSizer *labeledColumnImpl(wxWindow *parent, const wxString &label, wxWindow *widget,
                              wxSizer *content) {
  auto *col = new wxBoxSizer(wxVERTICAL);
  col->Add(makeBoldLabel(parent, label), 0, wxBOTTOM, 2);
  if (widget)
    col->Add(widget, 0, wxEXPAND);
  else if (content)
    col->Add(content, 0, wxEXPAND);
  return col;
}
} // namespace

wxBoxSizer *labeledColumn(wxWindow *parent, const wxString &label, wxWindow *widget) {
  return labeledColumnImpl(parent, label, widget, nullptr);
}

wxBoxSizer *labeledColumn(wxWindow *parent, const wxString &label, wxSizer *content) {
  return labeledColumnImpl(parent, label, nullptr, content);
}

} // namespace gui::ui
