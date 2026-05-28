#include "cli/content/integration-strings.h"
#include "cli/logic/navigation.h"
#include "cli/utils/console.h"
#include <iostream>

using cli::utils::clearScreen;
using cli::logic::userInterface;
using cli::content::printGoodbyeBanner;

int main() {
  while (true) {
    clearScreen();
    if (userInterface() == 0) {
      clearScreen();
      std::cout << printGoodbyeBanner;
      break;
    }
    std::cout << "\n\nPress Enter to return to menu...";
    std::cin.get();
  }

  return 0;
}
