#include "methods.h"
#include "riemann.h"

namespace engine {

double midpointRiemannSum(const Node *root, double a, double b, double tolerance,
                          int &segmentCount) {
  return adaptiveRiemannSum<midEdge>(root, a, b, tolerance, segmentCount);
}

} // namespace engine
