#include "window-strings.h"

namespace gui::content {

const std::string kWindowTitle = "Integration Calculator";

const std::string kFunctionLabel = "Function (e.g. sin(x)+x)";
const std::string kFunctionTooltip =
    "Supported: +, -, *, /, ^, sin, cos, tan, exp, ln, log, abs, sqrt, pi, e";
const std::string kLowerBoundLabel = "Lower bound (a)";
const std::string kUpperBoundLabel = "Upper bound (b)";
const std::string kPrecisionLabel = "Precision (Segments):";
const std::string kIntegrationRuleLabel = "Integration Rule:";

const std::string kDefaultFunction = "sin(x)";
const std::string kDefaultLowerBound = "-3.14159";
const std::string kDefaultUpperBound = "3.14159";

const std::string BUTTON_CALCULATE = "Calculate";
const std::string BUTTON_CLEAR = "Clear";
const std::string BUTTON_HISTORY = "History";
const std::string BUTTON_ZOOM_IN = "Zoom +";
const std::string kZoomOutButton = "Zoom -";
const std::string kResetViewButton = "Reset View";

const std::string kResultArea = "Area";
const std::string kResultMin = "Min Value";
const std::string kResultMax = "Max Value";
const std::string kResultMean = "Mean Value";
const std::string kResultRms = "RMS";
const std::string kResultDelta = "Delta (f(b)-f(a))";
const std::string kResultSegments = "Segments";

const std::string ERR_EMPTY_FUNCTION = "Please enter a function to compute.";
const std::string ERR_BOUND_EMPTY = "the field is empty";
const std::string ERR_BOUND_ORDER = "Lower bound (a) must be strictly less than upper bound (b).";
const std::string ERR_PARSE_FUNCTION = "Could not parse the mathematical expression.";
const std::string ERR_NONFINITE_RESULT =
    "The result is undefined over this interval. The function may have a "
    "singularity (e.g. division by zero) between the bounds.";

const std::string ERR_TITLE_INPUT = "Input Error";
const std::string ERR_TITLE_PARSE = "Parse Error";
const std::string kTitleEvaluationWarning = "Evaluation Warning";

const std::string SUCCESS_PARSED_LABEL = "Parsed as:";
const std::string SUCCESS_PARSED_AS = "—"; // em-dash placeholder

std::string ERR_BOUND_INVALID(const std::string &reason) {
  return "Invalid bound (" + reason +
         ").\nBounds may be a number or constant expression (e.g. pi, -pi/2, 2*e).";
}

} // namespace gui::content
