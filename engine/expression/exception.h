#ifndef ENGINE_EXPRESSION_EXCEPTION_H
#define ENGINE_EXPRESSION_EXCEPTION_H

#include <stdexcept>
#include <string>

namespace engine {

// EXCEPTION HANDLING: a small hierarchy of expression exceptions.
// ExpressionException is the common base, so a caller can catch every engine
// failure in one place; ParseException and EvaluationException distinguish the
// two stages (turning text into a tree, then evaluating that tree).
class ExpressionException : public std::runtime_error {
public:
  explicit ExpressionException(const std::string &message) : std::runtime_error(message) {
  }
};

// Thrown when an expression string cannot be parsed into a tree.
class ParseException : public ExpressionException {
public:
  explicit ParseException(const std::string &message) : ExpressionException(message) {
  }
};

// Thrown while evaluating a tree when the maths is undefined, e.g. a call to a
// function the engine does not know.
class EvaluationException : public ExpressionException {
public:
  explicit EvaluationException(const std::string &message) : ExpressionException(message) {
  }
};

} // namespace engine

#endif // ENGINE_EXPRESSION_EXCEPTION_H
