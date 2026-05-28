#include "utils.h"
#include "engine/expression/parser.h"
#include "engine/integration/integrator.h"
#include "engine/integration/stats.h"
#include <string>

static bool testConstantFunction() {
  // f(x) = 5 over [0,1]: min=max=mean=rms=5, delta=0
  engine::Node *root = engine::parseExpression("5");
  int segments = 0;
  double area = engine::integrate(engine::IntegrationMethod::Simpsons, root, 0.0, 1.0, 1e-6, segments);
  engine::IntegrationStats stats = engine::calculateStats(root, 0.0, 1.0, segments, area);
  delete root;

  bool ok = true;
  ok &= check(approxEqual(stats.getMinVal(),  5.0, 1e-6), "stats(5): min = 5",   stats.getMinVal(),  5.0);
  ok &= check(approxEqual(stats.getMaxVal(),  5.0, 1e-6), "stats(5): max = 5",   stats.getMaxVal(),  5.0);
  ok &= check(approxEqual(stats.getMeanVal(), 5.0, 1e-6), "stats(5): mean = 5",  stats.getMeanVal(), 5.0);
  ok &= check(approxEqual(stats.getDelta(),   0.0, 1e-6), "stats(5): delta = 0", stats.getDelta(),   0.0);
  return ok;
}

static bool testIsFinite() {
  engine::Node *root = engine::parseExpression("x^2");
  int segments = 0;
  double area = engine::integrate(engine::IntegrationMethod::Simpsons, root, 0.0, 1.0, 1e-6, segments);
  engine::IntegrationStats stats = engine::calculateStats(root, 0.0, 1.0, segments, area);
  delete root;
  return checkBool(stats.isFinite(), "stats(x^2) isFinite = true");
}

int main(int argc, char *argv[]) {
  if (argc < 2) {
    std::cerr << "usage: stats_tests <test-name>\n";
    return 2;
  }

  std::string name = argv[1];

  // clang-format off
  if (name == "constant_fn") return testConstantFunction() ? 0 : 1;
  if (name == "is_finite")   return testIsFinite()         ? 0 : 1;
  // clang-format on

  std::cerr << "unknown test: " << name << "\n";
  return 2;
}
