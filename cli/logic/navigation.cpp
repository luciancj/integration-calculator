#include "navigation.h"
#include "execution.h"
#include "../../engine/integration/integrator.h"
#include "../content/integration-strings.h"
#include "../content/menu-strings.h"
#include "../content/messages.h"
#include "../utils/console.h"
#include <expected>
#include <iostream>

namespace cli::logic {

using cli::utils::clearScreen;
using cli::utils::getSafeInput;
using cli::utils::printError;
using cli::utils::TextBlock;

namespace content = cli::content;

enum class MenuChoice : uint8_t {
  OK,
  BACK,
  CANCEL
};

// Read one line, strip a leading '/', and classify it.
// Returns CANCEL on EOF or "q", BACK if "back", OK otherwise (result in out).
static MenuChoice readChoice(std::string &out) {
  if (!getSafeInput(out))
    return MenuChoice::CANCEL;
  if (!out.empty() && out[0] == '/')
    out = out.substr(1);
  if (out == "q")
    return MenuChoice::CANCEL;
  if (out == "back")
    return MenuChoice::BACK;
  return MenuChoice::OK;
}

std::expected<engine::IntegrationMethod, std::string>
selectIntegrationMethod(const std::string &code) {
  if (code == "rr") return engine::IntegrationMethod::RightRiemann;
  if (code == "lr") return engine::IntegrationMethod::LeftRiemann;
  if (code == "mr") return engine::IntegrationMethod::MidpointRiemann;
  if (code == "tr") return engine::IntegrationMethod::Trapezoidal;
  if (code == "sr") return engine::IntegrationMethod::Simpsons;
  return std::unexpected(code);
}

const TextBlock *getMethodBanner(const std::string &methodCode) {
  if (methodCode == "rr") return &content::bannerRightRiemannSum;
  if (methodCode == "lr") return &content::bannerLeftRiemannSum;
  if (methodCode == "mr") return &content::bannerMidpointRiemannSum;
  if (methodCode == "tr") return &content::bannerTrapezoidalRule;
  if (methodCode == "sr") return &content::bannerSimpsonsRule;
  return nullptr;
}

// Interactive Mode: show sub-menu, dispatch to execution.
// Returns BACK if the user typed "back", CANCEL on "q"/EOF, OK when done.
static MenuChoice runInteractiveMenu() {
  std::cout << content::interactiveModeBanner;
  std::cout << "Enter an option in interactive mode: ";
  std::string input;
  MenuChoice entry = readChoice(input);
  if (entry != MenuChoice::OK)
    return entry;

  runInteractiveMethod(input);
  return MenuChoice::OK;
}

// Parameter Mode: show sub-menu, dispatch to execution.
// Returns BACK if the user typed "back", CANCEL on "q"/EOF, OK when done.
static MenuChoice runParameterMenu() {
  std::cout << content::parameterModeBanner;
  std::cout << "Enter an option in parameter mode: ";
  std::string input;
  MenuChoice entry = readChoice(input);
  if (entry != MenuChoice::OK)
    return entry;

  if (!selectIntegrationMethod(input)) {
    printError(content::ERR_INVALID_CHOICE);
    return MenuChoice::OK;
  }

  runParameterCsv(input);
  return MenuChoice::OK;
}

// Main UI loop: step 0 = main menu, step 1 = sub-menu.
// "back" at step 0 re-asks (no previous step); at step 1 returns to step 0.
// "q" or EOF exits at any point.
int userInterface() {
  std::cout << content::printIntegrationBanner;

  std::string modeInput;

  int step = 0;
  while (step < 2) {
    MenuChoice entry = MenuChoice::OK;
    std::string input;

    if (step == 0) {
      std::cout << content::welcomeMenu;
      std::cout << "Enter your choice: ";
      entry = readChoice(input);
    } else {
      if (modeInput == "i")
        entry = runInteractiveMenu();
      else
        entry = runParameterMenu();
    }

    if (entry == MenuChoice::CANCEL)
      return 0;

    if (entry == MenuChoice::BACK) {
      if (step > 0)
        step--;
      continue;
    }

    if (step == 0) {
      if (input == "e")
        return 0;
      if (input != "i" && input != "p") {
        printError(content::ERR_INVALID_CHOICE);
        continue;
      }
      modeInput = input;
      clearScreen();
    }

    step++;
  }

  return 1;
}

} // namespace cli::logic
