#include "methods.h"
#include <cmath>

namespace engine {

// Calculates Trapezoidal Rule adaptively
double trapezoidalRule(const Node *root, double lowerLimit, double upperLimit, double tolerance,
                       int &segmentCount) {
  int n = 10;
  double prev_sum = 0.0;
  double current_sum = 0.0;

  do {
    prev_sum = current_sum;
    current_sum = 0.0;
    double stepSize = (upperLimit - lowerLimit) / n;
    double sum = root->evaluate(lowerLimit) + root->evaluate(upperLimit);

    for (int i = 1; i < n; i++) {
      double x = lowerLimit + i * stepSize;
      sum += 2.0 * root->evaluate(x);
    }
    current_sum = (stepSize / 2.0) * sum;

    if (n > 4 && std::fabs(current_sum - prev_sum) < tolerance) {
      segmentCount = n;
      return current_sum;
    }
    n *= 2;
    // SAFETY: the cap stops the doubling if the tolerance is never met
    // (e.g. a singularity), so the loop can't run forever.
  } while (n <= 1000000);

  segmentCount = n;
  return current_sum;
}

} // namespace engine
