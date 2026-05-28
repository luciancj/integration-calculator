#ifndef CLI_INPUT_INPUT_PROVIDER_H
#define CLI_INPUT_INPUT_PROVIDER_H

#include "../../engine/integration/integrator.h"

namespace cli::input {

// Collects everything needed for one calculation: the interval bounds, the
// tolerance, and the function. At any prompt the user may type "back" to
// change a previous value or "q" to cancel.
//
// Returns true if all values were entered; then parameters and expr are
// filled in and the caller must delete expr. Returns false if the user
// cancelled or the input stream ended (EOF).
bool userInputForCalculation(engine::IntegrationParameters &parameters, engine::Node *&expression);

} // namespace cli::input

#endif
