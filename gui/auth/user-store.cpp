#include "user-store.h"

#include <cstdint>
#include <fstream>
#include <iomanip>
#include <sstream>

namespace gui::auth {

UserStore::UserStore(std::filesystem::path filePath) : filePath_(std::move(filePath)) {
  load();
}

bool UserStore::exists(const std::string &username) const {
  return hashByUsername_.find(username) != hashByUsername_.end();
}

bool UserStore::verify(const std::string &username, const std::string &password) const {
  auto it = hashByUsername_.find(username);
  if (it == hashByUsername_.end())
    return false;
  return it->second == hashPassword(password);
}

bool UserStore::registerUser(const std::string &username, const std::string &password) {
  if (exists(username))
    return false;
  hashByUsername_.emplace(username, hashPassword(password));
  save();
  return true;
}

// Tiny custom hash. NOT cryptographic — a real system would use bcrypt/argon2
// with a per-user salt. This is plenty for "don't store the password as
// plaintext in users.txt" and unlike std::hash it's deterministic across
// program runs (libc++ randomizes std::hash<string> per-process).
//
// Algorithm: FNV-1a, 64-bit. For each byte, XOR it into the accumulator and
// multiply by a fixed prime. Five lines, easy to inspect, well-known.
std::string UserStore::hashPassword(const std::string &password) {
  std::uint64_t h = 0xcbf29ce484222325ULL; // FNV-1a 64-bit offset basis
  for (unsigned char c : password) {
    h ^= c;
    h *= 0x100000001b3ULL; // FNV-1a 64-bit prime
  }
  std::ostringstream oss;
  oss << std::hex << std::setw(16) << std::setfill('0') << h;
  return oss.str();
}

void UserStore::saveSession(const std::string &username) const {
  std::ofstream out(filePath_.parent_path() / "session.txt", std::ios::trunc);
  if (out.is_open())
    out << username << '\n';
}

std::string UserStore::loadSession() const {
  std::ifstream in(filePath_.parent_path() / "session.txt");
  std::string username;
  if (in.is_open())
    std::getline(in, username);
  return username;
}

void UserStore::clearSession() const {
  std::error_code ec;
  std::filesystem::remove(filePath_.parent_path() / "session.txt", ec);
}

void UserStore::load() {
  std::ifstream in(filePath_);
  if (!in.is_open())
    return; // first run: no file yet

  std::string line;
  while (std::getline(in, line)) {
    auto sep = line.find(':');
    if (sep == std::string::npos)
      continue;
    hashByUsername_.emplace(line.substr(0, sep), line.substr(sep + 1));
  }
}

void UserStore::save() const {
  std::ofstream out(filePath_, std::ios::trunc);
  if (!out.is_open())
    return;
  for (const auto &[name, hash] : hashByUsername_) {
    out << name << ':' << hash << '\n';
  }
}

} // namespace gui::auth
