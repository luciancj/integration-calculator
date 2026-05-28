#ifndef CLI_LOGIC_NAVIGATION_H
#define CLI_LOGIC_NAVIGATION_H

#include "../../engine/integration/integrator.h"
#include "../utils/console.h"
#include <expected>
#include <string>

namespace cli::logic {

// Map a 2-letter user code ("rr", "lr", ...) to an IntegrationMethod.
// Returns the method on success, or an error string if the code is unknown.
std::expected<engine::IntegrationMethod, std::string>
selectIntegrationMethod(const std::string &code);

// Returns the banner TextBlock for a method code, or nullptr if unknown.
const cli::utils::TextBlock *getMethodBanner(const std::string &methodCode);

int userInterface();

} // namespace cli::logic

#endif
