#ifndef ENGINE_EXPRESSION_PARSER_H
#define ENGINE_EXPRESSION_PARSER_H

#include "tree.h"
#include <expected>
#include <string>

namespace engine {

// Parse expression text into a tree. Throws ParseException if the text is not a
// valid expression. The caller owns the returned tree and must delete it.
Node *parseExpression(const std::string &expression);

// Try to parse and evaluate a constant expression (no x). On success holds the
// value; on failure holds a message explaining why (bad syntax, unknown
// function, or non-finite result). This is a boundary adapter: it never throws,
// it turns the engine's exceptions into a value-or-reason result.
std::expected<double, std::string> evaluateConstant(const std::string &expression);

} // namespace engine
#endif
