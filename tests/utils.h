#ifndef TESTS_UTILS_H
#define TESTS_UTILS_H

#include <cmath>
#include <iostream>
#include <string>

inline bool approxEqual(double a, double b, double tolerance) {
  return std::fabs(a - b) <= tolerance;
}

inline bool check(bool passed, const std::string &label, double got, double expected) {
  if (passed)
    std::cout << "PASS: " << label << "\n";
  else
    std::cout << "FAIL: " << label << " (got " << got << ", expected " << expected << ")\n";
  return passed;
}

inline bool checkBool(bool passed, const std::string &label) {
  std::cout << (passed ? "PASS: " : "FAIL: ") << label << "\n";
  return passed;
}

#endif
