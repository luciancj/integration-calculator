#include "integrator.h"
#include "method/methods.h"

namespace engine {

// Integrate using the chosen method: pick the matching function and run it.
double integrate(IntegrationMethod method, const Node *root, double a, double b, double tolerance,
                 int &segmentCount) {
  switch (method) {
  case IntegrationMethod::RightRiemann:
    return rightRiemannSum(root, a, b, tolerance, segmentCount);

  case IntegrationMethod::LeftRiemann:
    return leftRiemannSum(root, a, b, tolerance, segmentCount);
  case IntegrationMethod::MidpointRiemann:
    return midpointRiemannSum(root, a, b, tolerance, segmentCount);
  case IntegrationMethod::Trapezoidal:
    return trapezoidalRule(root, a, b, tolerance, segmentCount);
  case IntegrationMethod::Simpsons:
    return simpsonsRule(root, a, b, tolerance, segmentCount);
  }
  return rightRiemannSum(root, a, b, tolerance, segmentCount); // fallback
}

// Fixed-segment integration: apply the chosen rule with exactly `segments`
// slices (no adaptive doubling). The formulas mirror the adaptive rules.
double integrateFixed(IntegrationMethod method, const Node *root, double a, double b, int segments) {
  if (segments <= 0)
    return 0.0;

  double h = (b - a) / segments;
  double sum = 0.0;

  switch (method) {
  case IntegrationMethod::RightRiemann:
    for (int i = 1; i <= segments; i++)
      sum += root->evaluate(a + i * h);
    return sum * h;

  case IntegrationMethod::LeftRiemann:
    for (int i = 0; i < segments; i++)
      sum += root->evaluate(a + i * h);
    return sum * h;

  case IntegrationMethod::MidpointRiemann:
    for (int i = 0; i < segments; i++)
      sum += root->evaluate(a + (i + 0.5) * h);
    return sum * h;

  case IntegrationMethod::Trapezoidal:
    sum = root->evaluate(a) + root->evaluate(b);
    for (int i = 1; i < segments; i++)
      sum += 2.0 * root->evaluate(a + i * h);
    return (h / 2.0) * sum;

  case IntegrationMethod::Simpsons:
    // Simpson's rule needs an even number of intervals.
    if (segments % 2 != 0)
      segments++;
    h = (b - a) / segments;
    sum = root->evaluate(a) + root->evaluate(b);
    for (int i = 1; i < segments; i++)
      sum += (i % 2 == 0 ? 2.0 : 4.0) * root->evaluate(a + i * h);
    return sum * h / 3.0;
  }
  return 0.0;
}

const char *methodName(IntegrationMethod method) {
  switch (method) {
  case IntegrationMethod::RightRiemann:    return "Right Riemann Sum";
  case IntegrationMethod::LeftRiemann:     return "Left Riemann Sum";
  case IntegrationMethod::MidpointRiemann: return "Midpoint Riemann Sum";
  case IntegrationMethod::Trapezoidal:     return "Trapezoidal Rule";
  case IntegrationMethod::Simpsons:        return "Simpson's Rule";
  }
  return "";
}

} // namespace engine
