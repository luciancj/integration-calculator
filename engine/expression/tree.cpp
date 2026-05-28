#include "tree.h"
#include "exception.h"
#include <cmath>

namespace engine {

// NumberNode: represents a constant number
NumberNode::NumberNode(double value) : numberValue_(value) {}

double NumberNode::evaluate(double x) const {
  (void)x;
  return numberValue_;
}

Node *NumberNode::clone() const {
  return new NumberNode(numberValue_);
}

std::string NumberNode::toString() const {
  return std::to_string(numberValue_);
}

double NumberNode::getValue() const {
  return numberValue_;
}

// VariableNode: represents the variable x
VariableNode::VariableNode(char variable) : variableName_(variable) {}

double VariableNode::evaluate(double x) const {
  return x;
}

Node *VariableNode::clone() const {
  return new VariableNode(variableName_);
}

std::string VariableNode::toString() const {
  return std::string(1, variableName_);
}

// OperatorNode: represents a binary operation (+, -, *, /, ^)
OperatorNode::OperatorNode(char op, Node *left, Node *right)
    : operation_(op), leftOperand_(left), rightOperand_(right) {}

OperatorNode::~OperatorNode() {
  delete leftOperand_;
  delete rightOperand_;
}

double OperatorNode::evaluate(double x) const {
  double leftVal = leftOperand_->evaluate(x);
  double rightVal = rightOperand_->evaluate(x);

  switch (operation_) {
  case '+': return leftVal + rightVal;
  case '-': return leftVal - rightVal;
  case '*': return leftVal * rightVal;
  case '/': return leftVal / rightVal;
  case '^': return std::pow(leftVal, rightVal);
  default:  return 0.0;
  }
}

Node *OperatorNode::clone() const {
  return new OperatorNode(operation_, leftOperand_->clone(), rightOperand_->clone());
}

std::string OperatorNode::toString() const {
  return "(" + leftOperand_->toString() + " " + std::string(1, operation_) + " " +
         rightOperand_->toString() + ")";
}

char OperatorNode::getOperator() const {
  return operation_;
}

// FunctionNode: represents a function call like sin(x), pow(x, 2)
FunctionNode::FunctionNode(const std::string &name, Node *argument, Node *secondArg)
    : functionName_(name), argumentNode_(argument), secondArgumentNode_(secondArg) {}

FunctionNode::~FunctionNode() {
  delete argumentNode_;
  delete secondArgumentNode_;
}

double FunctionNode::evaluate(double x) const {
  double childVal = argumentNode_->evaluate(x);

  if (functionName_ == "sin")  return std::sin(childVal);
  if (functionName_ == "cos")  return std::cos(childVal);
  if (functionName_ == "tan")  return std::tan(childVal);
  if (functionName_ == "log" || functionName_ == "ln") {
    if (childVal > 0) {
      return std::log(childVal);
    } else {
      return 0.0;
    }
  }
  if (functionName_ == "exp")  return std::exp(childVal);
  if (functionName_ == "sqrt") {
    if (childVal >= 0) {
      return std::sqrt(childVal);
    } else {
      return 0.0;
    }
  }
  if (functionName_ == "abs")  return std::fabs(childVal);
  if (functionName_ == "pow" && secondArgumentNode_)
    return std::pow(childVal, secondArgumentNode_->evaluate(x));

  throw EvaluationException("unknown function '" + functionName_ + "'");
}

Node *FunctionNode::clone() const {
  Node *clonedSecond;
  if (secondArgumentNode_) {
    clonedSecond = secondArgumentNode_->clone();
  } else {
    clonedSecond = nullptr;
  }
  return new FunctionNode(functionName_, argumentNode_->clone(), clonedSecond);
}

std::string FunctionNode::toString() const {
  std::string result = functionName_ + "(" + argumentNode_->toString();
  if (secondArgumentNode_)
    result += ", " + secondArgumentNode_->toString();
  result += ")";
  return result;
}

} // namespace engine
