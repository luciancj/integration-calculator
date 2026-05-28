#include "parser.h"
#include "exception.h"
#include <cmath>
#include <numbers>

// M_PI and M_E are POSIX extensions not available on MSVC — use C++20 <numbers> instead.
static constexpr double PI = std::numbers::pi;
static constexpr double EU = std::numbers::e;

namespace engine {

// I used AI to write the parser structure based on recursive descent parsing.
// See https://en.wikipedia.org/wiki/Recursive_descent_parser
// It separates parsing by precedence levels: each function handles one operator level
// by calling lower-precedence functions recursively.

// Character checks written by hand so they take a plain char with no casting,
// which avoids <cctype>. Fine because expressions are ASCII only.
static bool isDigitChar(char c) {
  return c >= '0' && c <= '9';
}
static bool isLetter(char c) {
  return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

// Global parser state: the input string and current position
static std::string s;
static int i;

// Return current character, or '\0' at end of string
static char ch() {
  if (i < (int)s.size()) {
    return s[i];
  } else {
    return '\0';
  }
}

// Skip whitespace in the input
static void skipSpaces() {
  while (ch() == ' ')
    i++;
}

// Forward declarations for recursive parsing
static Node *parseExpression();
static Node *parseTerm();

// Parse atomic values: numbers, variables, function calls, parentheses, unary minus
static Node *parseFactor() {
  skipSpaces();

  // Parenthesized expression
  if (ch() == '(') {
    i++;
    Node *n = parseExpression();
    if (ch() == ')')
      i++;
    return n;
  }

  // I used AI to design the unary minus handling. Convert -X to (-1) * X
  // so it works with the existing precedence rules.
  if (ch() == '-') {
    i++;
    Node *f = parseFactor();
    if (!f)
      return nullptr;
    return new OperatorNode('*', new NumberNode(-1), f);
  }

  // number parsing manually: extract digits one by one.
  // Multiply by 10 for each new digit on the left, multiply by 0.1 for decimals.
  if (isDigitChar(ch()) || ch() == '.') {
    double val = 0.0;
    while (isDigitChar(ch())) {
      val = val * 10 + (ch() - '0');
      i++;
    }
    if (ch() == '.') {
      i++;
      double factor = 0.1;
      while (isDigitChar(ch())) {
        val += (ch() - '0') * factor;
        factor *= 0.1;
        i++;
      }
    }
    return new NumberNode(val);
  }

  // Variable, constant, or function call
  if (isLetter(ch())) {
    int start = i;
    while (isLetter(ch()))
      i++;
    std::string name = s.substr(start, i - start);

    // Built-in constants
    if (name == "x")
      return new VariableNode('x');
    if (name == "pi" || name == "PI")
      return new NumberNode(PI);
    if (name == "e")
      return new NumberNode(EU);

    // I used AI to design function call parsing: check for '(' after the name,
    // then parse 1 or 2 comma-separated arguments.
    skipSpaces();
    if (ch() != '(')
      return nullptr;
    i++;

    Node *arg1 = parseExpression();
    if (!arg1)
      return nullptr;

    Node *arg2 = nullptr;
    if (ch() == ',') {
      i++;
      arg2 = parseExpression();
      if (!arg2) {
        delete arg1;
        return nullptr;
      }
    }

    if (ch() == ')')
      i++;
    return new FunctionNode(name, arg1, arg2);
  }

  return nullptr;
}

// Parse exponentiation (right-associative)
static Node *parseExponent() {
  Node *left = parseFactor();
  if (!left)
    return nullptr;
  skipSpaces();
  if (ch() == '^') {
    i++;
    Node *right = parseExponent();
    if (!right) {
      delete left;
      return nullptr;
    }
    return new OperatorNode('^', left, right);
  }
  return left;
}

// Parse multiplication and division (left-associative)
static Node *parseTerm() {
  Node *left = parseExponent();
  if (!left)
    return nullptr;
  skipSpaces();
  while (ch() == '*' || ch() == '/') {
    char op = ch();
    i++;
    Node *right = parseExponent();
    if (!right) {
      delete left;
      return nullptr;
    }
    left = new OperatorNode(op, left, right);
    skipSpaces();
  }
  return left;
}

// Parse addition and subtraction (lowest precedence)
static Node *parseExpression() {
  Node *left = parseTerm();
  if (!left)
    return nullptr;
  skipSpaces();
  while (ch() == '+' || ch() == '-') {
    char op = ch();
    i++;
    Node *right = parseTerm();
    if (!right) {
      delete left;
      return nullptr;
    }
    left = new OperatorNode(op, left, right);
    skipSpaces();
  }
  return left;
}

// Parse a complete expression string into an expression tree
Node *parseExpression(const std::string &expression) {
  s = expression;
  i = 0;
  Node *root = parseExpression();
  skipSpaces();
  // Leftover characters mean the text was only partly understood.
  if (i < (int)s.size()) {
    delete root;
    throw ParseException("unexpected characters in expression: '" + expression + "'");
  }
  if (!root)
    throw ParseException("could not parse expression: '" + expression + "'");
  return root;
}

// Parse and evaluate a constant expression (no x variable). Boundary adapter:
// turns the engine's exceptions into a value-or-reason result for callers that
// treat a non-number as a normal case (e.g. a CSV header field). The error side
// keeps the reason the old bool return discarded.
std::expected<double, std::string> evaluateConstant(const std::string &expression) {
  Node *root = nullptr;
  try {
    root = parseExpression(expression); // may throw ParseException
    double value = root->evaluate(0.0); // may throw EvaluationException
    delete root;
    // Reject non-finite results (inf, nan)
    if (!std::isfinite(value))
      return std::unexpected("expression did not evaluate to a finite number");
    return value;
  } catch (const ExpressionException &e) {
    delete root; // safe even if null: parseExpression threw before assigning
    return std::unexpected(std::string(e.what()));
  }
}

} // namespace engine
