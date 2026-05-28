#include "messages.h"

namespace cli::content {
// clang-format off
const std::string ERR_INVALID_CHOICE        = "Invalid choice. Please try again.";
const std::string ERR_UNKNOWN_METHOD        = "Error: Unknown method selected.";
const std::string ERR_READ_FILENAME_FAILED  = "Error: Failed to read filename.";
const std::string ERR_OUTPUT_FAILED         = "Error: Could not create output.csv.";
const std::string ERR_EMPTY_INPUT           = "Input was empty. Please try again.";
const std::string ERR_INVALID_NUMBER        = "That is not a valid number. Please try again.";
const std::string ERR_INVALID_EXPRESSION    = "Could not read that as a function of x. Please try again.";
const std::string ERR_INVALID_BOUNDS        = "The start (a) must be less than the end (b). Please try again.";
const std::string ERR_INVALID_TOLERANCE     = "Tolerance must be a whole number from 1 to 15. Please try again.";
const std::string ERR_CANCELLED             = "Cancelled. Returning to the menu.";
const std::string WARN_NONFINITE_RESULT     = "Warning: the result is undefined over this interval (the function may have a singularity).";
// clang-format on

std::string ERR_OPEN_FILE(const std::string &fileName) {
  return "Error: Could not open file '" + fileName + "'.";
}

std::string ERR_SKIP_FILE_LINE(int lineNum, const std::string &line) {
  return "Warning: Skipping malformed line " + std::to_string(lineNum) + ": " + line;
}

std::string ERR_EVALUATION_FAILED(const std::string &detail) {
  return "Could not evaluate the function: " + detail + ".";
}

std::string ERR_INVALID_NUMBER_REASON(const std::string &reason) {
  return "That is not a valid number (" + reason + "). Please try again.";
}

std::string SUCCESS_FILE_COUNT(int count) {
  return "Successfully processed " + std::to_string(count) + " function(s).";
}

} // namespace cli::content
