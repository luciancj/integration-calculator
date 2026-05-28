#ifndef CLI_UTILS_CONSOLE_H
#define CLI_UTILS_CONSOLE_H

#include <iostream>
#include <string>
#include <vector>

namespace engine {
class IntegrationStats;
}

namespace cli::utils {

inline const std::string ANSI_GREEN = "\x1B[32m";
inline const std::string ANSI_RED = "\x1B[31m";
inline const std::string ANSI_RESET = "\x1B[0m";

class TextBlock {
public:
  TextBlock() = default;

  TextBlock(const std::vector<std::string> &rows) : rows_(rows) {}

  int rowCount() const {
    return (int)rows_.size();
  }
  const std::string &rowAt(int index) const {
    return rows_[index];
  }

  friend std::ostream &operator<<(std::ostream &os, const TextBlock &block);

private:
  std::vector<std::string> rows_;
};

void clearScreen();
bool getSafeInput(std::string &line);
void displayResults(const engine::IntegrationStats &stats);

void printError(const std::string &message);
void printWarning(const std::string &message);
void printSuccess(const std::string &message);

std::ostream &operator<<(std::ostream &os, const TextBlock &block);

extern std::string choice;

} // namespace cli::utils

#endif
