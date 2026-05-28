#ifndef CLI_LOGIC_EXECUTION_H
#define CLI_LOGIC_EXECUTION_H

#include <string>

namespace cli::logic {

void runInteractiveMethod(const std::string &option);
void runParameterCsv(const std::string &methodCode);

} // namespace cli::logic

#endif
