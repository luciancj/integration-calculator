#ifndef ENGINE_INTEGRATION_METHOD_METHODS_H
#define ENGINE_INTEGRATION_METHOD_METHODS_H

#include "../../expression/tree.h"

namespace engine {

// Each method approximates the integral of root from a to b. It starts with a
// small number of segments and doubles until two successive estimates agree
// within tolerance, then reports how many segments it ended up using through
// segmentCount. Every method is defined in its own .cpp file in this folder.
double rightRiemannSum(const Node *root, double a, double b, double tolerance, int &segmentCount);
double leftRiemannSum(const Node *root, double a, double b, double tolerance, int &segmentCount);
double midpointRiemannSum(const Node *root, double a, double b, double tolerance, int &segmentCount);
double trapezoidalRule(const Node *root, double lowerLimit, double upperLimit, double tolerance,
                       int &segmentCount);
double simpsonsRule(const Node *root, double a, double b, double tol, int &segments);

} // namespace engine

#endif // ENGINE_INTEGRATION_METHOD_METHODS_H
