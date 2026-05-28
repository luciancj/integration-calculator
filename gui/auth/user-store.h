#ifndef GUI_AUTH_USER_STORE_H
#define GUI_AUTH_USER_STORE_H

#include <filesystem>
#include <string>
#include <unordered_map>

namespace gui::auth {

// Tiny on-disk user store. Each line of the backing file is "username:hashHex".
// Hashing uses a small hand-written FNV-1a — NOT cryptographic; for demo
// purposes only. A real system would use bcrypt / argon2 / scrypt with a
// per-user salt.
class UserStore {
public:
  explicit UserStore(std::filesystem::path filePath);

  bool exists(const std::string &username) const;
  bool verify(const std::string &username, const std::string &password) const;

  // Adds a new user. Returns false if the username is already taken.
  bool registerUser(const std::string &username, const std::string &password);

  // Persist / retrieve / clear the last logged-in username so the app can
  // skip the login dialog on next launch.
  void saveSession(const std::string &username) const;
  std::string loadSession() const;
  void clearSession() const;

private:
  static std::string hashPassword(const std::string &password);
  void load();
  void save() const;

  std::filesystem::path filePath_;
  std::unordered_map<std::string, std::string> hashByUsername_;
};

} // namespace gui::auth

#endif
