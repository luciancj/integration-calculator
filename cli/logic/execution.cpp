#include "execution.h"
#include "../../engine/csv/processor.h"
#include "../../engine/expression/exception.h"
#include "../../engine/integration/integrator.h"
#include "../../engine/integration/stats.h"
#include "../content/menu-strings.h"
#include "../content/messages.h"
#include "../input/input-provider.h"
#include "../utils/console.h"
#include "navigation.h"
#include <fstream>
#include <iostream>

namespace cli::logic {

using cli::utils::clearScreen;
using cli::utils::displayResults;
using cli::utils::getSafeInput;
using cli::utils::printError;
using cli::utils::printSuccess;
using cli::utils::printWarning;

using cli::input::userInputForCalculation;

namespace content = cli::content;

static void executeMethod(engine::IntegrationMethod method, const engine::Node *root,
                          const engine::IntegrationParameters &parameter) {
  try {
    int segmentCount = 0;
    double result =
        engine::integrate(method, root, parameter.getLowerLimit(), parameter.getUpperLimit(),
                          parameter.getTolerance(), segmentCount);
    engine::IntegrationStats stats = engine::calculateStats(
        root, parameter.getLowerLimit(), parameter.getUpperLimit(), segmentCount, result);
    if (!stats.isFinite())
      printWarning(content::WARN_NONFINITE_RESULT);
    displayResults(stats);
  } catch (const engine::EvaluationException &e) {
    printError(content::ERR_EVALUATION_FAILED(e.what()));
  }
}

// Run one interactive calculation for the chosen method code:
// show the banner, collect inputs, then integrate and display.
void runInteractiveMethod(const std::string &option) {
  if (option.empty()) {
    printError(content::ERR_INVALID_CHOICE);
    return;
  }

  const auto *banner = getMethodBanner(option);
  if (banner == nullptr) {
    printError(content::ERR_INVALID_CHOICE);
    return;
  }

  clearScreen();
  std::cout << *banner;
  std::cout << "(type 'back' to change a previous value, or 'q' to cancel)" << '\n';

  engine::IntegrationParameters parameters;
  engine::Node *expression = nullptr;
  if (!userInputForCalculation(parameters, expression))
    return;

  auto method = selectIntegrationMethod(option);
  if (method) {
    executeMethod(*method, expression, parameters);
  } else {
    printError(content::ERR_UNKNOWN_METHOD);
  }
  delete expression;
}

// Run one batch CSV calculation for the chosen method code.
void runParameterCsv(const std::string &methodCode) {
  auto method = selectIntegrationMethod(methodCode);
  if (!method) {
    printError(content::ERR_INVALID_CHOICE);
    return;
  }

  std::cout << '\n';
  std::cout << content::askFileNameBanner;

  // Keep asking for a filename until one opens, so a simple typo does not
  // throw the user all the way back to the main menu. Give up only if the
  // input stream ends (EOF).
  std::ifstream inputFile;
  while (true) {
    std::cout << "Enter filename: ";
    std::string fileName;
    if (!getSafeInput(fileName)) {
      printError(content::ERR_READ_FILENAME_FAILED);
      return;
    }

    inputFile.open(fileName);
    if (inputFile.is_open())
      break;

    printError(content::ERR_OPEN_FILE(fileName));
    inputFile.clear();
  }

  std::ofstream outputFile("output.csv");
  if (!outputFile.is_open()) {
    printError(content::ERR_OUTPUT_FAILED);
    return;
  }

  engine::CSVResult result = engine::processCsv(inputFile, outputFile, *method);

  std::cout << '\n';
  printSuccess(content::SUCCESS_FILE_COUNT(result.processed));
  std::cout << "Results written to 'output.csv'" << '\n';
}

} // namespace cli::logic
