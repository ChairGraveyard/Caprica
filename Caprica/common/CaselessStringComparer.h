#pragma once

#include <cctype>
#include <cstdint>
#include <cstring>
#include <functional>
#include <map>
#include <string>
#include <unordered_map>
#include <unordered_set>

namespace caprica {

struct CaselessStringComparer final : public std::function<bool(std::string, std::string)>
{
  bool operator()(const char* const lhs, const char* const rhs) const {
    return _stricmp(lhs, rhs) < 0;
  }

  bool operator()(const std::string &lhs, const std::string &rhs) const {
    return _stricmp(lhs.c_str(), rhs.c_str()) < 0;
  }
};

struct CaselessStringHasher final : public std::function<size_t(std::string)>
{
  size_t operator()(const char* k) const {
    return doCaselessHash(k, strlen(k));
  }
  size_t operator()(const std::string& k) const {
    return doCaselessHash(k.c_str(), k.size());
  }

private:
  static size_t doCaselessHash(const char* k, size_t len);
};

struct CaselessStringEqual final : public std::function<bool(std::string, std::string)>
{
  bool operator()(const char* lhs, const char* rhs) const {
    return _stricmp(lhs, rhs) == 0;
  }

  bool operator()(const std::string& lhs, const std::string& rhs) const {
    return _stricmp(lhs.c_str(), rhs.c_str()) == 0;
  }
};

inline bool idEq(const char* a, const char* b) {
  return !_stricmp(a, b);
}
inline bool idEq(const char* a, const std::string& b) {
  return idEq(a, b.c_str());
}
inline bool idEq(const std::string& a, const char* b) {
  return idEq(a.c_str(), b);
}
inline bool idEq(const std::string& a, const std::string& b) {
  return idEq(a.c_str(), b.c_str());
}

struct CaselessIdentifierHasher final : public std::function<size_t(std::string)>
{
  size_t operator()(const char* k) const {
    return doIdentifierHash(k, strlen(k));
  }
  size_t operator()(const std::string& k) const {
    return doIdentifierHash(k.c_str(), k.size());
  }
private:
  static size_t doIdentifierHash(const char* k, size_t len);
};

//using CaselessStringHasher = CaselessIdentifierHasher;

struct CaselessIdentifierEqual final : public std::function<bool(std::string, std::string)>
{
  bool operator()(const char* lhs, const char* rhs) const {
    return idEq(lhs, rhs);
  }

  bool operator()(const std::string& lhs, const std::string& rhs) const {
    return idEq(lhs.c_str(), rhs.c_str());
  }
};

void identifierToLower(std::string& str);

template<typename T>
using caseless_unordered_set = typename std::unordered_set<T, CaselessStringHasher, CaselessStringEqual>;
template<typename K, typename V>
using caseless_unordered_map = typename std::unordered_map<K, V, CaselessStringHasher, CaselessStringEqual>;
template<typename K, typename V>
using caseless_map = typename std::map<K, V, CaselessStringComparer>;

template<typename T>
using caseless_unordered_identifier_set = typename std::unordered_set<T, CaselessIdentifierHasher, CaselessIdentifierEqual>;
template<typename K, typename V>
using caseless_unordered_identifier_map = typename std::unordered_map<K, V, CaselessIdentifierHasher, CaselessIdentifierEqual>;

}
