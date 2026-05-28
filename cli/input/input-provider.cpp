#include "input-provider.h"
#include "../../engine/expression/exception.h"
#include "../../engine/expression/parser.h"
#include "../content/messages.h"
#include "../utils/console.h"
#include <cmath>
#include <iostream>
#include <sstream>
#include <string>

namespace cli::input {

using namespace std;
using namespace cli::utils;
namespace content = cli::content;

// What one prompt produced: a value, a request to go back one step, or a
// request to cancel the whole calculation.
enum class Request : uint8_t {
  OK,
  BACK,
  CANCEL
}; // Enum 'Request' uses a larger base type ('int', size: 4 bytes) than necessary for its value
   // set, consider using 'std::uint8_t' (1 byte) as the base type to reduce its size

static bool isBack(const string &line) {
  return line == "back";
}
static bool isCancel(const string &line) {
  return line == "q";
}

// Show a prompt and read one line. Detects the "back" and "q" keywords and
// the end of input (EOF, treated as cancel).
static Request readPromptLine(const string &prompt, string &line) {
  cout << prompt;
  if (!getline(cin, line))
    return Request::CANCEL; // input stream closed (EOF)
  if (isCancel(line))
    return Request::CANCEL;
  if (isBack(line))
    return Request::BACK;
  return Request::OK;
}

// Read an interval bound: a plain number or a constant expression like "pi".
static Request readBound(const string &prompt, double &out) {
  while (true) {
    string line;
    Request entry = readPromptLine(prompt, line);
    if (entry != Request::OK)
      return entry;

    if (line.empty()) {
      printError(content::ERR_EMPTY_INPUT);
      continue;
    }

    auto value = engine::evaluateConstant(line);
    if (value) {
      out = *value;
      return Request::OK;
    }

    printError(content::ERR_INVALID_NUMBER_REASON(value.error()));
  }
}

// Read a whole number for the tolerance.
static Request readTolerance(const string &prompt, int &out) {
  while (true) {
    string line;
    Request entry = readPromptLine(prompt, line);
    if (entry != Request::OK)
      return entry;

    if (line.empty()) {
      printError(content::ERR_EMPTY_INPUT);
      continue;
    }

    // The extra read checks that nothing (like letters) follows the number.
    istringstream stream(line);
    int value;
    char leftover;
    if (stream >> value && !(stream >> leftover)) {
      out = value;
      return Request::OK;
    }

    printError(content::ERR_INVALID_NUMBER);
  }
}

// Read a function of x and parse it into an expression tree.
static Request readExpression(const string &prompt, engine::Node *&out) {
  while (true) {
    string line;
    Request entry = readPromptLine(prompt, line);
    if (entry != Request::OK)
      return entry;

    if (line.empty()) {
      printError(content::ERR_EMPTY_INPUT);
      continue;
    }

    try {
      out = engine::parseExpression(line);
      return Request::OK;
    } catch (const engine::ParseException &) {
      printError(content::ERR_INVALID_EXPRESSION);
    }
  }
}

bool userInputForCalculation(engine::IntegrationParameters &parameters, engine::Node *&expression) {
  double lower = 0.0;
  double upper = 0.0;
  int precision = 4;
  engine::Node *root = nullptr;

  // A small wizard. Each step reads one value; "back" moves to the previous
  // step so a wrong value can be corrected, and "q" or EOF cancels.
  int step = 0;
  while (step < 4) {
    Request entry = Request::OK;
    if (step == 0) {
      entry = readBound("Enter the start of the interval (a): ", lower);
    } else if (step == 1) {
      entry = readBound("Enter the end of the interval (b): ", upper);
    } else if (step == 2) {
      entry = readTolerance("Enter tolerance (integer, e.g., 4 for 0.0001): ", precision);
    } else {
      entry = readExpression("Enter a function in terms of x (e.g., 'sin(x)+2*x'): ", root);
    }

    if (entry == Request::CANCEL) {
      cout << content::ERR_CANCELLED << '\n';
      delete root; // null unless a function was already entered
      return false;
    }

    if (entry == Request::BACK) {
      if (step > 0)
        step--; // step 0 has no previous step, so just re-ask it
      continue;
    }

    // After reading the end bound, make sure the interval is in order.
    if (step == 1 && lower >= upper) {
      printError(content::ERR_INVALID_BOUNDS);
      continue; // stay on this step and re-ask b
    }

    // The tolerance is used as 10^(-precision), so the precision must be a
    // small positive number. A double only holds about 15 significant digits.
    if (step == 2 && (precision < 1 || precision > 15)) {
      printError(content::ERR_INVALID_TOLERANCE);
      continue; // stay on this step and re-ask the tolerance
    }

    step++;
  }

  parameters.setLowerLimit(lower);
  parameters.setUpperLimit(upper);
  parameters.setTolerance(pow(10.0, -precision));
  expression = root;
  return true;
}

} // namespace cli::input
