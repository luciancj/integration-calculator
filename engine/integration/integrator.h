#ifndef ENGINE_INTEGRATION_INTEGRATOR_H
#define ENGINE_INTEGRATION_INTEGRATOR_H

#include "../expression/tree.h"

namespace engine {

// OPERATOR OVERLOADING (local): member operator== on a class.
class IntegrationParameters {
public:
  IntegrationParameters() = default;
  IntegrationParameters(double lowerLimit, double upperLimit, double tolerance)
      : lowerLimit_(lowerLimit), upperLimit_(upperLimit), tolerance_(tolerance) {
  }

  double getLowerLimit() const {
    return lowerLimit_;
  }
  double getUpperLimit() const {
    return upperLimit_;
  }
  double getTolerance() const {
    return tolerance_;
  }

  void setLowerLimit(double value) {
    lowerLimit_ = value;
  }
  void setUpperLimit(double value) {
    upperLimit_ = value;
  }
  void setTolerance(double value) {
    tolerance_ = value;
  }

  // Local operator overload: compare two parameter sets
  bool operator==(const IntegrationParameters &other) const {
    return lowerLimit_ == other.lowerLimit_ && upperLimit_ == other.upperLimit_ &&
           tolerance_ == other.tolerance_;
  }

  bool operator!=(const IntegrationParameters &other) const {
    return !(*this == other);
  }

private:
  double lowerLimit_ = 0.0;
  double upperLimit_ = 0.0;
  double tolerance_ = 0.0001;
};

// Single source of truth for "which integration method to use".
// CLI converts its own input form (string code, dropdown
// index, etc.) into one of these values, then look up the function.
enum class IntegrationMethod {
  RightRiemann,
  LeftRiemann,
  MidpointRiemann,
  Trapezoidal,
  Simpsons,
};

// Integrate root from a to b using the chosen method. Reports how many
// segments were used through segmentCount, and returns the area. The
// individual methods live in the method/ folder.
double integrate(IntegrationMethod method, const Node *root, double a, double b,
                 double tolerance, int &segmentCount);

// Integrate with an exact number of segments instead of adaptively to a
// tolerance. Used by the GUI, where a slider chooses the segment count.
double integrateFixed(IntegrationMethod method, const Node *root, double a, double b,
                      int segments);

// Human-readable label for a method (used by the GUI and history logging).
const char *methodName(IntegrationMethod method);

} // namespace engine

#endif // ENGINE_INTEGRATION_INTEGRATOR_H
