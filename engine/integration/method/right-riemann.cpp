#include "methods.h"
#include "riemann.h"

namespace engine {

double rightRiemannSum(const Node *root, double a, double b, double tolerance, int &segmentCount) {
  return adaptiveRiemannSum<rightEdge>(root, a, b, tolerance, segmentCount);
}

} // namespace engine
