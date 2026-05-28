#ifndef GUI_UI_WIDGETS_H
#define GUI_UI_WIDGETS_H

#include <functional>
#include <wx/wx.h>

// Small widget-builder helpers. wxWidgets constructors take many positional
// arguments most of which are just defaults; these wrappers hide that noise so
// call sites read like a UI manifest:
//
//   auto *username = makeInputField(this, "");
//   auto *submit   = makeButton(this, "Log In", onSubmit);
//
// instead of:
//
//   auto *username = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition,
//                                   wxDefaultSize, 0, wxDefaultValidator);
//
// Each helper does one thing. Compose them at the call site.
namespace gui::ui {

// Plain text label using the system default font.
wxStaticText *makeLabel(wxWindow *parent, const wxString &text);

// Bold section label (used for "Function:", "Lower bound (a)", etc.).
wxStaticText *makeBoldLabel(wxWindow *parent, const wxString &text);

// Greyed-out label, optionally wrapped to a max width (in DIP, -1 = no wrap).
wxStaticText *makeMutedLabel(wxWindow *parent, const wxString &text, int wrapDip = -1);

// Single-line text field with an optional default value and minimum width
// (in DIP, -1 = no override).
wxTextCtrl *makeInputField(wxWindow *parent, const wxString &defaultValue = wxEmptyString,
                           int minWidthDip = -1);

// Password field (echoes asterisks).
wxTextCtrl *makePasswordField(wxWindow *parent);

// Push button bound to a callback. Pass nullptr for the callback if you want
// to bind it later or use the default wx event routing.
wxButton *makeButton(wxWindow *parent, const wxString &text,
                     std::function<void()> onClick = nullptr,
                     int id = wxID_ANY);

// Horizontal integer slider with a minimum width override (in DIP).
wxSlider *makeIntSlider(wxWindow *parent, int min, int max, int initial,
                        int minWidthDip = -1);

// Choice (drop-down) with the given items.
wxChoice *makeChoice(wxWindow *parent, const wxArrayString &items, int initialSelection = 0,
                     int minWidthDip = -1);

// Vertical sizer that stacks a bold label above the given widget. Useful for
// labelled inputs ("Lower bound (a)" + text field, "Precision:" + slider, …).
wxBoxSizer *labeledColumn(wxWindow *parent, const wxString &label, wxWindow *widget);

// Same shape, but the second child is itself a sizer (e.g. slider + value).
wxBoxSizer *labeledColumn(wxWindow *parent, const wxString &label, wxSizer *content);

} // namespace gui::ui

#endif
