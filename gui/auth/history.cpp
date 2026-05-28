#include "history.h"

#include <chrono>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <sstream>

namespace gui::auth {

namespace {

constexpr char kSep = '\t';

std::string nowTimestamp() {
  auto now = std::chrono::system_clock::now();
  std::time_t t = std::chrono::system_clock::to_time_t(now);
  std::tm tm{};
#ifdef _WIN32
  localtime_s(&tm, &t);
#else
  localtime_r(&t, &tm);
#endif
  std::ostringstream oss;
  oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
  return oss.str();
}

} // namespace

History::History(std::filesystem::path filePath) : filePath_(std::move(filePath)) {}

void History::append(const HistoryEntry &entry) {
  std::ofstream out(filePath_, std::ios::app);
  if (!out.is_open())
    return;
  // Caller may leave timestamp empty — fill it in here so logging is one-call.
  const std::string &ts = entry.timestamp.empty() ? nowTimestamp() : entry.timestamp;
  out << ts << kSep << entry.function << kSep << entry.lower << kSep << entry.upper << kSep
      << entry.segments << kSep << entry.method << kSep << entry.area << '\n';
}

void History::clear() {
  std::error_code ec;
  std::filesystem::remove(filePath_, ec);
}

std::vector<HistoryEntry> History::readAll() const {
  std::vector<HistoryEntry> entries;
  std::ifstream in(filePath_);
  if (!in.is_open())
    return entries;

  std::string line;
  while (std::getline(in, line)) {
    std::istringstream iss(line);
    HistoryEntry e;
    std::getline(iss, e.timestamp, kSep);
    std::getline(iss, e.function, kSep);
    std::string lower, upper, segments, method, area;
    std::getline(iss, lower, kSep);
    std::getline(iss, upper, kSep);
    std::getline(iss, segments, kSep);
    std::getline(iss, e.method, kSep);
    std::getline(iss, area, kSep);
    try {
      e.lower = std::stod(lower);
      e.upper = std::stod(upper);
      e.segments = std::stoi(segments);
      e.area = std::stod(area);
    } catch (...) {
      continue; // skip malformed line
    }
    entries.push_back(std::move(e));
  }
  return entries;
}

} // namespace gui::auth
