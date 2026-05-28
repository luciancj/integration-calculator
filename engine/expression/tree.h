#ifndef ENGINE_EXPRESSION_TREE_H
#define ENGINE_EXPRESSION_TREE_H

#include <string>

namespace engine {

// MULTIPLE INHERITANCE: Node inherits from both Evaluable and
// Printable interfaces.
//
// POLYMORPHISM: evaluate(), clone(), toString() are pure virtual
// functions overridden by each derived class.
//
// INHERITANCE: NumberNode, VariableNode, OperatorNode, and
// FunctionNode all derive from the abstract Node base class.

// Interface for evaluable expressions
class Evaluable {
public:
  virtual double evaluate(double x) const = 0;
  virtual ~Evaluable() = default;
};

// Interface for printable objects
class Printable {
public:
  virtual std::string toString() const = 0;
  virtual ~Printable() = default;
};

// Abstract base class: Multiple Inheritance from Evaluable + Printable
class Node : public Evaluable, public Printable {
public:
  virtual Node *clone() const = 0;
  ~Node() override = default;
};

// Derived Classes (Inheritance + Polymorphism)

class NumberNode : public Node {
public:
  explicit NumberNode(double value);
  double evaluate(double x) const override;
  Node *clone() const override;
  std::string toString() const override;
  double getValue() const;

private:
  double numberValue_;
};

class VariableNode : public Node {
public:
  explicit VariableNode(char variable);
  double evaluate(double x) const override;
  Node *clone() const override;
  std::string toString() const override;

private:
  char variableName_;
};

class OperatorNode : public Node {
public:
  OperatorNode(char op, Node *left, Node *right);
  ~OperatorNode() override;
  double evaluate(double x) const override;
  Node *clone() const override;
  std::string toString() const override;
  char getOperator() const;

private:
  char operation_;
  Node *leftOperand_;
  Node *rightOperand_;
};

class FunctionNode : public Node {
public:
  FunctionNode(const std::string &name, Node *argument, Node *secondArg = nullptr);
  ~FunctionNode() override;
  double evaluate(double x) const override;
  Node *clone() const override;
  std::string toString() const override;

private:
  std::string functionName_;
  Node *argumentNode_;
  Node *secondArgumentNode_;
};

} // namespace engine

#endif
