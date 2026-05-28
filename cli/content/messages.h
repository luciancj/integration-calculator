#ifndef CLI_CONTENT_MESSAGES_H
#define CLI_CONTENT_MESSAGES_H

#include <string>

namespace cli::content {

extern const std::string ERR_INVALID_CHOICE;
extern const std::string ERR_UNKNOWN_METHOD;
extern const std::string ERR_READ_FILENAME_FAILED;
extern const std::string ERR_OUTPUT_FAILED;
extern const std::string ERR_EMPTY_INPUT;
extern const std::string ERR_INVALID_NUMBER;
extern const std::string ERR_INVALID_EXPRESSION;
extern const std::string ERR_INVALID_BOUNDS;
extern const std::string ERR_INVALID_TOLERANCE;
extern const std::string ERR_CANCELLED;
extern const std::string WARN_NONFINITE_RESULT;

std::string ERR_OPEN_FILE(const std::string &fileName);
std::string ERR_SKIP_FILE_LINE(int lineNum, const std::string &line);
std::string ERR_EVALUATION_FAILED(const std::string &detail);
std::string ERR_INVALID_NUMBER_REASON(const std::string &reason);
std::string SUCCESS_FILE_COUNT(int count);

} // namespace cli::content

#endif
