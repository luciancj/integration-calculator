#ifndef ENGINE_INTEGRATION_METHOD_RIEMANN_H
#define ENGINE_INTEGRATION_METHOD_RIEMANN_H

#include "../../expression/tree.h"
#include <cmath>

namespace engine {

// Sample-point policies — each picks the x value to evaluate within [x0, x1].
inline double leftEdge(double x0, double /*x1*/)   { return x0; }
inline double rightEdge(double /*x0*/, double x1)  { return x1; }
inline double midEdge(double x0, double x1)        { return (x0 + x1) / 2.0; }

// Shared adaptive Riemann sum. Doubles n until two successive estimates agree
// within tolerance or n exceeds the safety cap.
template <auto SamplePoint>
double adaptiveRiemannSum(const Node *root, double a, double b, double tolerance,
                          int &segmentCount) {
  int n = 10;
  double prev = 0.0;
  double curr = 0.0;

  do {
    prev = curr;
    curr = 0.0;
    double h = (b - a) / n;

    for (int i = 0; i < n; i++) {
      double x0 = a + i * h;
      double x1 = x0 + h;
      curr += root->evaluate(SamplePoint(x0, x1));
    }
    curr *= h;

    if (n > 4 && std::fabs(curr - prev) < tolerance) {
      segmentCount = n;
      return curr;
    }
    n *= 2;
  } while (n <= 1000000);

  segmentCount = n;
  return curr;
}

} // namespace engine

#endif
