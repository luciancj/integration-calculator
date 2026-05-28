#include "methods.h"
#include "riemann.h"

namespace engine {

double leftRiemannSum(const Node *root, double a, double b, double tolerance, int &segmentCount) {
  return adaptiveRiemannSum<leftEdge>(root, a, b, tolerance, segmentCount);
}

} // namespace engine
