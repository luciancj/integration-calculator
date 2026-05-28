#include "console.h"
#include "../../engine/integration/stats.h"
#include <iomanip>
#include <iostream>

// Box-drawing characters reference:
// URL: https://en.wikipedia.org/wiki/Box-drawing_characters

namespace cli::utils {

using namespace std;

string choice;

void clearScreen() {
  cout << "\033[H\033[J" << flush;
}

bool getSafeInput(string &line) {
  if (!getline(cin, line)) {
    return false;
  }
  return true;
}

// OPERATOR OVERLOADING (global): ostream << TextBlock.
// Outputs all rows of a TextBlock to the given stream.
ostream &operator<<(ostream &os, const TextBlock &block) {
  for (int i = 0; i < block.rowCount(); i++) {
    os << block.rowAt(i) << '\n';
  }
  return os;
}

void printError(const string &message) {
  cerr << ANSI_RED << message << ANSI_RESET << '\n';
}

void printWarning(const string &message) {
  cerr << ANSI_RED << message << ANSI_RESET << '\n';
}

void printSuccess(const string &message) {
  cout << ANSI_GREEN << message << ANSI_RESET << '\n';
}

// Displays formatted results table
void displayResults(const engine::IntegrationStats &stats) {
  cout << '\n';
  cout << "╔════════════════════════╦══════════════════╗" << '\n';
  cout << "║ DESCRIPTION            ║ VALUE            ║" << '\n';
  cout << "╠════════════════════════╬══════════════════╣" << '\n';

  cout << "║ Result Area            ║ " << ANSI_GREEN << left << setw(16) << fixed
       << setprecision(6) << stats.getArea() << ANSI_RESET << " ║" << '\n';
  cout << "╠════════════════════════╬══════════════════╣" << '\n';

  cout << "║ Min Value              ║ " << ANSI_GREEN << left << setw(16) << fixed
       << setprecision(6) << stats.getMinVal() << ANSI_RESET << " ║" << '\n';
  cout << "╠════════════════════════╬══════════════════╣" << '\n';

  cout << "║ Max Value              ║ " << ANSI_GREEN << left << setw(16) << fixed
       << setprecision(6) << stats.getMaxVal() << ANSI_RESET << " ║" << '\n';
  cout << "╠════════════════════════╬══════════════════╣" << '\n';

  cout << "║ Mean Value             ║ " << ANSI_GREEN << left << setw(16) << fixed
       << setprecision(6) << stats.getMeanVal() << ANSI_RESET << " ║" << '\n';
  cout << "╠════════════════════════╬══════════════════╣" << '\n';

  cout << "║ Root mean square       ║ " << ANSI_GREEN << left << setw(16) << fixed
       << setprecision(6) << stats.getRmsVal() << ANSI_RESET << " ║" << '\n';
  cout << "╠════════════════════════╬══════════════════╣" << '\n';

  cout << "║ Delta (f(b)-f(a))      ║ " << ANSI_GREEN << left << setw(16) << fixed
       << setprecision(6) << stats.getDelta() << ANSI_RESET << " ║" << '\n';
  cout << "╚════════════════════════╩══════════════════╝" << '\n';
  cout << '\n';
}

} // namespace cli::utils
