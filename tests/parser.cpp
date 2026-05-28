#include "engine/expression/parser.h"
#include "engine/expression/exception.h"
#include "engine/expression/tree.h"
#include "utils.h"
#include <expected>
#include <string>

static bool testParseConstant() {
  auto result = engine::evaluateConstant("2+3*4");
  if (!result) {
    std::cout << "FAIL: parse_constant could not evaluate: " << result.error() << "\n";
    return false;
  }
  return check(approxEqual(*result, 14.0, 1e-9), "2+3*4 = 14", *result, 14.0);
}

static bool testParseConstantPi() {
  auto result = engine::evaluateConstant("pi");
  if (!result) {
    std::cout << "FAIL: parse_constant_pi: " << result.error() << "\n";
    return false;
  }
  return check(approxEqual(*result, M_PI, 1e-9), "pi evaluates to M_PI", *result, M_PI);
}

static bool testParseConstantE() {
  auto result = engine::evaluateConstant("e");
  if (!result) {
    std::cout << "FAIL: parse_constant_e: " << result.error() << "\n";
    return false;
  }
  return check(approxEqual(*result, M_E, 1e-9), "e evaluates to M_E", *result, M_E);
}

static bool testParseInvalid() {
  bool threw = false;
  try {
    engine::Node *root = engine::parseExpression("((((");
    delete root;
  } catch (const engine::ParseException &) {
    threw = true;
  }
  return checkBool(threw, "parseExpression throws ParseException on malformed input");
}

static bool testParseEmpty() {
  bool threw = false;
  try {
    engine::Node *root = engine::parseExpression("");
    delete root;
  } catch (const engine::ParseException &) {
    threw = true;
  }
  return checkBool(threw, "parseExpression throws ParseException on empty input");
}

static bool testEvaluateExpression() {
  engine::Node *root = engine::parseExpression("x^2");
  double value = root->evaluate(3.0);
  delete root;
  return check(approxEqual(value, 9.0, 1e-9), "x^2 at x=3 = 9", value, 9.0);
}

static bool testEvaluateSin() {
  engine::Node *root = engine::parseExpression("sin(x)");
  double value = root->evaluate(M_PI / 2.0);
  delete root;
  return check(approxEqual(value, 1.0, 1e-9), "sin(pi/2) = 1", value, 1.0);
}

static bool testEvaluateNegative() {
  engine::Node *root = engine::parseExpression("-x");
  double value = root->evaluate(5.0);
  delete root;
  return check(approxEqual(value, -5.0, 1e-9), "-x at x=5 = -5", value, -5.0);
}

static bool testEvaluateUnknownFunction() {
  bool threw = false;
  try {
    engine::Node *root = engine::parseExpression("blah(x)");
    root->evaluate(1.0);
    delete root;
  } catch (const engine::EvaluationException &) {
    threw = true;
  } catch (const engine::ParseException &) {
    threw = true;
  }
  return checkBool(threw, "unknown function throws at parse or eval time");
}

int main(int argc, char *argv[]) {
  if (argc < 2) {
    std::cerr << "usage: parser_tests <test-name>\n";
    return 2;
  }

  std::string name = argv[1];

  // clang-format off
  if (name == "parse_constant")      return testParseConstant()          ? 0 : 1;
  if (name == "parse_constant_pi")   return testParseConstantPi()        ? 0 : 1;
  if (name == "parse_constant_e")    return testParseConstantE()         ? 0 : 1;
  if (name == "parse_invalid")       return testParseInvalid()           ? 0 : 1;
  if (name == "parse_empty")         return testParseEmpty()             ? 0 : 1;
  if (name == "evaluate_expression") return testEvaluateExpression()     ? 0 : 1;
  if (name == "evaluate_sin")        return testEvaluateSin()            ? 0 : 1;
  if (name == "evaluate_negative")   return testEvaluateNegative()       ? 0 : 1;
  if (name == "evaluate_unknown_fn") return testEvaluateUnknownFunction()? 0 : 1;
  // clang-format on

  std::cerr << "unknown test: " << name << "\n";
  return 2;
}
