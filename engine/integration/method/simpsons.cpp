#include "methods.h"
#include <cmath>

namespace engine {

// Calculates Simpson's Rule (1/3) adaptively
double simpsonsRule(const Node *root, double a, double b, double tol, int &segments) {
  int n = 10;
  double prev_sum = 0.0;
  double current_sum = 0.0;

  // Initial calculation
  double h = (b - a) / n;
  prev_sum = root->evaluate(a) + root->evaluate(b);
  for (int i = 1; i < n; i++) {
    double x = a + i * h;
    double fx = root->evaluate(x);
    if (i % 2 == 0) {
      prev_sum += 2.0 * fx;
    } else {
      prev_sum += 4.0 * fx;
    }
  }
  prev_sum *= h / 3.0;

  // SAFETY: the cap stops the doubling if the tolerance is never met
  // (e.g. a singularity), so the loop can't run forever.
  while (n < 1000000) {
    n *= 2;
    h = (b - a) / n;
    current_sum = root->evaluate(a) + root->evaluate(b);
    for (int i = 1; i < n; i++) {
      double x = a + i * h;
      double fx = root->evaluate(x);
      if (i % 2 == 0) {
        current_sum += 2.0 * fx;
      } else {
        current_sum += 4.0 * fx;
      }
    }
    current_sum *= h / 3.0;

    if (std::fabs(current_sum - prev_sum) < tol) {
      segments = n;
      return current_sum;
    }
    prev_sum = current_sum;
  }
  segments = n;
  return current_sum;
}

} // namespace engine
