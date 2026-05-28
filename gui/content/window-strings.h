#ifndef GUI_CONTENT_MAIN_WINDOW_STRINGS_H
#define GUI_CONTENT_MAIN_WINDOW_STRINGS_H

#include <string>

// All user-facing strings for the main window in one place. Mirrors the
// pattern used by cli/content/menu-strings & friends — one file you can grep
// or translate without hunting through layout/event-handler code.
namespace gui::content {

// Window
extern const std::string kWindowTitle;

// Input section
extern const std::string kFunctionLabel;
extern const std::string kFunctionTooltip;
extern const std::string kLowerBoundLabel;
extern const std::string kUpperBoundLabel;
extern const std::string kPrecisionLabel;
extern const std::string kIntegrationRuleLabel;

// Default field values shown on first launch / after Clear
extern const std::string kDefaultFunction;
extern const std::string kDefaultLowerBound;
extern const std::string kDefaultUpperBound;

// Buttons
extern const std::string BUTTON_CALCULATE;
extern const std::string BUTTON_CLEAR;
extern const std::string BUTTON_HISTORY;
extern const std::string BUTTON_ZOOM_IN;
extern const std::string kZoomOutButton;
extern const std::string kResetViewButton;

// Result row labels
extern const std::string kResultArea;
extern const std::string kResultMin;
extern const std::string kResultMax;
extern const std::string kResultMean;
extern const std::string kResultRms;
extern const std::string kResultDelta;
extern const std::string kResultSegments;

// Error messages
extern const std::string ERR_EMPTY_FUNCTION;
extern const std::string ERR_BOUND_ORDER;
extern const std::string ERR_PARSE_FUNCTION;
extern const std::string ERR_NONFINITE_RESULT;

// Bound that could not be read, with the specific reason from the parser.
std::string ERR_BOUND_INVALID(const std::string &reason);
// Reason used when a bound field is left blank.
extern const std::string ERR_BOUND_EMPTY;

// Error dialog titles
extern const std::string ERR_TITLE_INPUT;
extern const std::string ERR_TITLE_PARSE;
extern const std::string kTitleEvaluationWarning;

// "Parsed as" / "Recognized" label shown next to the function input
extern const std::string SUCCESS_PARSED_LABEL;
extern const std::string SUCCESS_PARSED_AS;

} // namespace gui::content

#endif
