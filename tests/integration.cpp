#include "engine/expression/parser.h"
#include "engine/integration/integrator.h"
#include "utils.h"
#include <string>

using engine::IntegrationMethod;

static bool testMethod(IntegrationMethod method, const std::string &label) {
  engine::Node *root = engine::parseExpression("x^2");
  int segments = 0;
  double area = engine::integrate(method, root, 0.0, 1.0, 1e-6, segments);
  delete root;
  return check(approxEqual(area, 1.0 / 3.0, 1e-3), label + ": x^2 over [0,1] = 1/3", area,
               1.0 / 3.0);
}

static bool testRightRiemann() {
  return testMethod(IntegrationMethod::RightRiemann, "right_riemann");
}
static bool testLeftRiemann() {
  return testMethod(IntegrationMethod::LeftRiemann, "left_riemann");
}
static bool testMidpointRiemann() {
  return testMethod(IntegrationMethod::MidpointRiemann, "midpoint");
}
static bool testTrapezoidal() {
  return testMethod(IntegrationMethod::Trapezoidal, "trapezoidal");
}
static bool testSimpsons() {
  return testMethod(IntegrationMethod::Simpsons, "simpsons");
}

static bool testLinear() {
  // integral of x over [0,2] = 2
  engine::Node *root = engine::parseExpression("x");
  int segments = 0;
  double area = engine::integrate(IntegrationMethod::Trapezoidal, root, 0.0, 2.0, 1e-6, segments);
  delete root;
  return check(approxEqual(area, 2.0, 1e-3), "x over [0,2] = 2", area, 2.0);
}

static bool testConstant() {
  // integral of 3 over [0,4] = 12
  engine::Node *root = engine::parseExpression("3");
  int segments = 0;
  double area = engine::integrate(IntegrationMethod::Simpsons, root, 0.0, 4.0, 1e-6, segments);
  delete root;
  return check(approxEqual(area, 12.0, 1e-3), "3 over [0,4] = 12", area, 12.0);
}

static bool testSin() {
  // integral of sin(x) over [0, pi] = 2
  engine::Node *root = engine::parseExpression("sin(x)");
  int segments = 0;
  double area = engine::integrate(IntegrationMethod::Simpsons, root, 0.0, M_PI, 1e-6, segments);
  delete root;
  return check(approxEqual(area, 2.0, 1e-3), "sin(x) over [0,pi] = 2", area, 2.0);
}

static bool testDegenerate() {
  // integral over [a,a] = 0
  engine::Node *root = engine::parseExpression("x^2");
  int segments = 0;
  double area = engine::integrate(IntegrationMethod::Simpsons, root, 1.0, 1.0, 1e-6, segments);
  delete root;
  return check(approxEqual(area, 0.0, 1e-9), "x^2 over [1,1] = 0 (degenerate interval)", area, 0.0);
}

int main(int argc, char *argv[]) {
  if (argc < 2) {
    std::cerr << "usage: integration_tests <test-name>\n";
    return 2;
  }

  std::string name = argv[1];

  // clang-format off
  if (name == "right_riemann")  return testRightRiemann()    ? 0 : 1;
  if (name == "left_riemann")   return testLeftRiemann()     ? 0 : 1;
  if (name == "midpoint")       return testMidpointRiemann() ? 0 : 1;
  if (name == "trapezoidal")    return testTrapezoidal()     ? 0 : 1;
  if (name == "simpsons")       return testSimpsons()        ? 0 : 1;
  if (name == "linear")         return testLinear()          ? 0 : 1;
  if (name == "constant")       return testConstant()        ? 0 : 1;
  if (name == "sin")            return testSin()             ? 0 : 1;
  if (name == "degenerate")     return testDegenerate()      ? 0 : 1;
  // clang-format on

  std::cerr << "unknown test: " << name << "\n";
  return 2;
}
