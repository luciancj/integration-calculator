#ifndef GUI_AUTH_HISTORY_H
#define GUI_AUTH_HISTORY_H

#include <filesystem>
#include <string>
#include <vector>

namespace gui::auth {

struct HistoryEntry {
  std::string timestamp; // ISO-8601, local time
  std::string function;
  double lower = 0.0;
  double upper = 0.0;
  int segments = 0;
  std::string method;
  double area = 0.0;
};

// Per-user calculation log. Entries are appended one-per-line to the backing
// file (TSV-ish). Cheap to write, easy to read.
class History {
public:
  explicit History(std::filesystem::path filePath);

  void append(const HistoryEntry &entry);
  std::vector<HistoryEntry> readAll() const;
  void clear();

private:
  std::filesystem::path filePath_;
};

} // namespace gui::auth

#endif
