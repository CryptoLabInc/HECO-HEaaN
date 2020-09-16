#include <stack>
#include <cstdio>

#include "ast_opt/ast/AbstractNode.h"
#include "ast_opt/ast/AbstractStatement.h"
#include "ast_opt/ast/AbstractExpression.h"
#include "ast_opt/ast/AbstractTarget.h"
#include "ast_opt/ast/BinaryExpression.h"
#include "ast_opt/ast/Block.h"
#include "ast_opt/ast/ExpressionList.h"
#include "ast_opt/ast/For.h"
#include "ast_opt/ast/Function.h"
#include "ast_opt/ast/FunctionParameter.h"
#include "ast_opt/ast/If.h"
#include "ast_opt/ast/IndexAccess.h"
#include "ast_opt/ast/Literal.h"
#include "ast_opt/ast/Return.h"
#include "ast_opt/ast/UnaryExpression.h"
#include "ast_opt/ast/Variable.h"
#include "ast_opt/ast/Assignment.h"
#include "ast_opt/ast/VariableDeclaration.h"
#include "ast_opt/parser/Errors.h"
#include "ast_opt/parser/Parser.h"
#include "ast_opt/parser/PushBackStream.h"

using std::to_string;

std::unique_ptr<AbstractNode> Parser::parse(std::string s) {

  // Setup Tokenizer from String
  stork::get_character get = [&s]() {
    if (s.empty()) {
      return (char) EOF;
    } else {
      char c = s.at(0);
      s.erase(0, 1);
      return c;
    }
  };
  stork::PushBackStream stream(&get);
  stork::tokens_iterator it(stream);

  auto block = std::make_unique<Block>();
  // Parse statements until end of file
  while (!it->isEof()) {
    block->appendStatement(std::unique_ptr<AbstractStatement>(parseStatement(it)));
  }

  return std::move(block);
}

AbstractStatement *Parser::parseStatement(stork::tokens_iterator &it) {
  if (it->isReservedToken()) {
    switch (it->get_reserved_token()) {
      case stork::reservedTokens::kw_for:return parseForStatement(it);
      case stork::reservedTokens::kw_if:return parseIfStatement(it);
      case stork::reservedTokens::kw_return:return parseReturnStatement(it);
      case stork::reservedTokens::open_curly:return parseBlockStatement(it);
      case stork::reservedTokens::kw_public: return parseFunctionStatement(it);

      // it starts with a data type (e.g., int, float)
      case stork::reservedTokens::kw_bool:
      case stork::reservedTokens::kw_char:
      case stork::reservedTokens::kw_int:
      case stork::reservedTokens::kw_float:
      case stork::reservedTokens::kw_double:
      case stork::reservedTokens::kw_string:
      case stork::reservedTokens::kw_void:
        return parseVariableDeclarationStatement(it);
      default:
        // has to be an assignment
        return parseAssignmentStatement(it);

    }
  } else {
    // it start with an identifier -> must be an assignment
    return parseAssignmentStatement(it);
  }
}

bool isBinaryOperator(const stork::tokens_iterator &it) {
  return
      it->isReservedToken() &&
          (
              it->hasValue(stork::reservedTokens::add) ||
                  it->hasValue(stork::reservedTokens::sub) ||
                  it->hasValue(stork::reservedTokens::concat) ||
                  it->hasValue(stork::reservedTokens::mul) ||
                  it->hasValue(stork::reservedTokens::div) ||
                  it->hasValue(stork::reservedTokens::idiv) ||
                  it->hasValue(stork::reservedTokens::mod) ||
                  it->hasValue(stork::reservedTokens::bitwise_and) ||
                  it->hasValue(stork::reservedTokens::bitwise_or) ||
                  it->hasValue(stork::reservedTokens::bitwise_xor) ||
                  it->hasValue(stork::reservedTokens::shiftl) ||
                  it->hasValue(stork::reservedTokens::shiftr) ||
                  it->hasValue(stork::reservedTokens::logical_and) ||
                  it->hasValue(stork::reservedTokens::logical_or) ||
                  it->hasValue(stork::reservedTokens::eq) ||
                  it->hasValue(stork::reservedTokens::ne) ||
                  it->hasValue(stork::reservedTokens::lt) ||
                  it->hasValue(stork::reservedTokens::gt) ||
                  it->hasValue(stork::reservedTokens::le) ||
                  it->hasValue(stork::reservedTokens::ge)
          );
}

bool isUnaryOperator(const stork::tokens_iterator &it) {
  return
      it->isReservedToken() &&
          (
              it->hasValue(stork::reservedTokens::logical_not) ||
                  it->hasValue(stork::reservedTokens::bitwise_not)
          );
}

bool isPostFixOperator(const stork::tokens_iterator &it) {
  return
      it->isReservedToken() && (it->hasValue(stork::reservedTokens::inc) || it->hasValue(stork::reservedTokens::dec));
}

bool isLiteral(stork::tokens_iterator &it) {
  return it->isBool() || it->isChar() || it->isFloat() || it->isDouble() || it->isInteger() || it->isString();
}

AbstractExpression *Parser::parseExpression(stork::tokens_iterator &it) {

  // if it begins with an "{" it must be an expression list which cannot be part of a greater expression
  if (it->hasValue(stork::reservedTokens::open_curly)) {
    return parseExpressionList(it);
  }

  // Shunting-yard algorithm: Keep a stack of operands and check precedence when you see an operator
  std::stack<AbstractExpression *, std::vector<AbstractExpression *>> operands;
  std::stack<Operator, std::vector<Operator>> operator_stack;

  bool running = true;
  while (running) {
    if (isBinaryOperator(it)) {
      Operator op1 = parseOperator(it);
      while (!operator_stack.empty()) {
        Operator op2 = operator_stack.top();
        if ((!op1.isRightAssociative() && comparePrecedence(op1, op2)==0) || comparePrecedence(op1, op2) < 0) {
          operator_stack.pop();
          AbstractExpression *rhs = operands.top();
          operands.pop();
          AbstractExpression *lhs = operands.top();
          operands.pop();
          operands.push(new BinaryExpression(std::unique_ptr<AbstractExpression>(lhs),
                                             op2,
                                             std::unique_ptr<AbstractExpression>(rhs)));
        } else {
          break;
        }
      }
      operator_stack.push(op1);
    } else if (isUnaryOperator(it)) {
      operator_stack.push(parseOperator(it));
    } else if (isPostFixOperator(it)) {
      Operator op(ArithmeticOp::ADDITION);
      if (it->hasValue(stork::reservedTokens::inc)) {
        // already an add
      } else if (it->hasValue(stork::reservedTokens::dec)) {
        op = Operator(ArithmeticOp::SUBTRACTION);
      } else {
        throw stork::parsingError("Unexpected Postfix Operator", it->getLineNumber(), it->getCharIndex());
      }
      if (operands.empty()) {
        throw stork::expectedSyntaxError("operand for postfix operator", it->getLineNumber(), it->getCharIndex());
      } else {
        auto exp = operands.top();
        operands.pop();
        operands
            .push(new BinaryExpression(std::unique_ptr<AbstractExpression>(exp), op, std::make_unique<LiteralInt>(1)));
      }
    } else if (isLiteral(it)) {
      operands.push(parseLiteral(it));
    } else if (it->isIdentifier()) {
      // When we see an identifier, it could be a variable or a more general IndexAccess
      operands.push(parseTarget(it));
    } else if (it->hasValue(stork::reservedTokens::open_round)) {
      // If we see an (, we have nested expressions going on, so use recursion.
      parseTokenValue(it, stork::reservedTokens::open_round);
      operands.push(parseExpression(it));
      parseTokenValue(it, stork::reservedTokens::close_round);
    } else {
      // Stop parsing tokens as soon as we see a closing ), a semicolon or anything else
      running = false;
    }

    // Check if we have a right-associative operator (currently only unary supported) ready to go on the stack
    if (!operator_stack.empty() && operator_stack.top().isRightAssociative()) {
      if (!operator_stack.top().isUnary()) {
        throw stork::parsingError("Cannot handle non-unary right-associative operators!",
                                  it->getLineNumber(),
                                  it->getCharIndex());
      } else {
        Operator op = operator_stack.top();
        operator_stack.pop();
        if (operands.empty()) {
          throw stork::expectedSyntaxError("Operand for Unary Operator to work on",
                                           it->getLineNumber(),
                                           it->getCharIndex());
        } else {
          AbstractExpression *exp = operands.top();
          operands.pop();
          operands.push(new UnaryExpression(std::unique_ptr<AbstractExpression>(exp), op));
        }
      }
    }

  } //end of while loop

  // cleanup any remaining operators
  while (!operator_stack.empty()) {
    Operator op = operator_stack.top();
    operator_stack.pop();

    // has to be binary?
    if (op.isUnary()) {
      throw stork::unexpectedSyntaxError("Unresolved Unary Operator", it->getLineNumber(), it->getCharIndex());
    } else {
      // Try to get two operands
      if (operands.size() < 2) {
        throw stork::unexpectedSyntaxError("Missing at least one Operand for Binary Operator",
                                           it->getLineNumber(),
                                           it->getCharIndex());
      } else {
        auto e1 = operands.top();
        operands.pop();
        auto e2 = operands.top();
        operands.pop();
        operands.push(new BinaryExpression(std::unique_ptr<AbstractExpression>(e1),
                                           op,
                                           std::unique_ptr<AbstractExpression>(e2)));
      }
    }
  }

  if (operands.empty()) {
    throw stork::unexpectedSyntaxError("Empty Expression", it->getLineNumber(), it->getCharIndex());
  } else if (operands.size()==1) {
    return operands.top();
  } else {
    throw stork::unexpectedSyntaxError("Unresolved Operands", it->getLineNumber(), it->getCharIndex());
  }
}

AbstractTarget *Parser::parseTarget(stork::tokens_iterator &it) {
  //Any valid target must begin with a Variable as its "root"
  Variable *v = parseVariable(it);

  std::vector<AbstractExpression *> indices;
  // if the next token is a "[" we need to keep on parsing
  while (it->hasValue(stork::reservedTokens::open_square)) {
    parseTokenValue(it, stork::reservedTokens::open_square);
    indices.push_back(parseExpression(it));
    parseTokenValue(it, stork::reservedTokens::close_square);
  }

  if (indices.empty()) {
    return v;
  } else {
    auto cur = new IndexAccess(std::unique_ptr<AbstractTarget>(v), std::unique_ptr<AbstractExpression>(indices[0]));
    for (size_t i = 1; i < indices.size(); ++i) {
      cur = new IndexAccess(std::unique_ptr<AbstractTarget>(cur), std::unique_ptr<AbstractExpression>(indices[i]));
    }
    return cur;
  }
}

AbstractExpression *Parser::parseLiteral(stork::tokens_iterator &it) {
  AbstractExpression *l = nullptr;
  if (it->isString()) {
    l = new LiteralString(it->getString());
  } else if (it->isDouble()) {
    l = new LiteralDouble(it->getDouble());
  } else if (it->isFloat()) {
    l = new LiteralFloat(it->getFloat());
  } else if (it->isChar()) {
    l = new LiteralChar(it->getChar());
  } else if (it->isInteger()) {
    l = new LiteralInt(it->getInteger());
  } else if (it->isBool()) {
    l = new LiteralBool(it->getBool());
  } else {
    throw stork::unexpectedSyntaxError(to_string(it->getValue()), it->getLineNumber(), it->getCharIndex());
  }
  ++it;
  return l;
}

ExpressionList *Parser::parseExpressionList(stork::tokens_iterator &it) {
  parseTokenValue(it, stork::reservedTokens::open_curly);
  std::vector<std::unique_ptr<AbstractExpression>> expressions;
  while (true) {
    expressions.push_back(std::unique_ptr<AbstractExpression>(parseExpression(it)));
    if (it->hasValue(stork::reservedTokens::comma)) {
      parseTokenValue(it, stork::reservedTokens::comma);
    } else {
      break;
    }
  }
  parseTokenValue(it, stork::reservedTokens::close_curly);
  return new ExpressionList(std::move(expressions));
}

Variable *Parser::parseVariable(stork::tokens_iterator &it) {
  auto identifier = parseIdentifier(it);
  auto variable = std::make_unique<Variable>(identifier);
  return new Variable(identifier);
}

Operator Parser::parseOperator(stork::tokens_iterator &it) {
  if (it->isReservedToken()) {
    if (it->hasValue(stork::reservedTokens::add)) {
      ++it;
      return Operator(ArithmeticOp::ADDITION);
    } else if (it->hasValue(stork::reservedTokens::sub)) {
      ++it;
      return Operator(ArithmeticOp::SUBTRACTION);
    } else if (it->hasValue(stork::reservedTokens::concat)) {
      throw stork::unexpectedSyntaxError("concatenation (not supported in AST)",
                                         it->getLineNumber(),
                                         it->getCharIndex());
    } else if (it->hasValue(stork::reservedTokens::mul)) {
      ++it;
      return Operator(ArithmeticOp::MULTIPLICATION);
    } else if (it->hasValue(stork::reservedTokens::div)) {
      ++it;
      return Operator(ArithmeticOp::DIVISION);
    } else if (it->hasValue(stork::reservedTokens::idiv)) {
      throw stork::unexpectedSyntaxError("integer division (not supported in AST)",
                                         it->getLineNumber(),
                                         it->getCharIndex());
    } else if (it->hasValue(stork::reservedTokens::mod)) {
      ++it;
      return Operator(ArithmeticOp::MODULO);
    } else if (it->hasValue(stork::reservedTokens::bitwise_and)) {
      ++it;
      return Operator(LogicalOp::BITWISE_AND);
    } else if (it->hasValue(stork::reservedTokens::bitwise_or)) {
      ++it;
      return Operator(LogicalOp::BITWISE_OR);
    } else if (it->hasValue(stork::reservedTokens::bitwise_xor)) {
      ++it;
      return Operator(LogicalOp::BITWISE_XOR);
    } else if (it->hasValue(stork::reservedTokens::shiftl)) {
      throw stork::unexpectedSyntaxError("shift left (not supported in AST)", it->getLineNumber(), it->getCharIndex());
    } else if (it->hasValue(stork::reservedTokens::shiftr)) {
      throw stork::unexpectedSyntaxError("shift right (not supported in AST)", it->getLineNumber(), it->getCharIndex());
    } else if (it->hasValue(stork::reservedTokens::logical_and)) {
      ++it;
      return Operator(LogicalOp::LOGICAL_AND);
    } else if (it->hasValue(stork::reservedTokens::logical_or)) {
      ++it;
      return Operator(LogicalOp::LOGICAL_OR);
    } else if (it->hasValue(stork::reservedTokens::eq)) {
      ++it;
      return Operator(LogicalOp::EQUAL);
    } else if (it->hasValue(stork::reservedTokens::ne)) {
      ++it;
      return Operator(LogicalOp::NOTEQUAL);
    } else if (it->hasValue(stork::reservedTokens::lt)) {
      ++it;
      return Operator(LogicalOp::LESS);
    } else if (it->hasValue(stork::reservedTokens::gt)) {
      ++it;
      return Operator(LogicalOp::GREATER);
    } else if (it->hasValue(stork::reservedTokens::le)) {
      ++it;
      return Operator(LogicalOp::LESS_EQUAL);
    } else if (it->hasValue(stork::reservedTokens::ge)) {
      ++it;
      return Operator(LogicalOp::GREATER_EQUAL);
    } else if (it->hasValue(stork::reservedTokens::logical_not)) {
      ++it;
      return Operator(UnaryOp::LOGICAL_NOT);
    } else if (it->hasValue(stork::reservedTokens::bitwise_not)) {
      ++it;
      return Operator(UnaryOp::BITWISE_NOT);
    }
  }
  // If we get here, it wasn't an operator
  throw stork::unexpectedSyntaxError(to_string(it->getValue()), it->getLineNumber(), it->getCharIndex());
}

/// consume token "value" and throw error if something different
void Parser::parseTokenValue(stork::tokens_iterator &it, const stork::token_value &value) {
  if (it->hasValue(value)) {
    ++it;
    return;
  }
  throw stork::expectedSyntaxError(to_string(value), it->getLineNumber(), it->getCharIndex());
}

Datatype Parser::parseDatatype(stork::tokens_iterator &it) {
  if (!it->isReservedToken()) {
    throw stork::unexpectedSyntaxError(to_string(it->getValue()), it->getLineNumber(), it->getCharIndex());
  }

  bool isSecret = false;
  if (it->hasValue(stork::reservedTokens::kw_secret)) {
    isSecret = true;
    parseTokenValue(it, stork::reservedTokens::kw_secret);
  }

  // just a placeholder as value-less constructor does not exist
  Datatype datatype(Type::VOID);
  switch (it->get_reserved_token()) {
    case stork::reservedTokens::kw_bool:datatype = Datatype(Type::BOOL, isSecret);
      break;
    case stork::reservedTokens::kw_char:datatype = Datatype(Type::CHAR, isSecret);
      break;
    case stork::reservedTokens::kw_int:datatype = Datatype(Type::INT, isSecret);
      break;
    case stork::reservedTokens::kw_float:datatype = Datatype(Type::FLOAT, isSecret);
      break;
    case stork::reservedTokens::kw_double:datatype = Datatype(Type::DOUBLE, isSecret);
      break;
    case stork::reservedTokens::kw_string:datatype = Datatype(Type::STRING, isSecret);
      break;
    case stork::reservedTokens::kw_void:datatype = Datatype(Type::VOID);
      break;
    default:throw stork::unexpectedSyntaxError(to_string(it->getValue()), it->getLineNumber(), it->getCharIndex());
  }

  ++it;

  return datatype;
}

std::string Parser::parseIdentifier(stork::tokens_iterator &it) {
  if (!it->isIdentifier()) {
    throw stork::unexpectedSyntaxError(to_string(it->getValue()), it->getLineNumber(), it->getCharIndex());
  }
  std::string ret = it->getIdentifier().name;
  ++it;
  return ret;
}

FunctionParameter *Parser::parseFunctionParameter(stork::tokens_iterator &it) {
  auto datatype = parseDatatype(it);
  auto identifier = parseIdentifier(it);

  auto functionParameter = new FunctionParameter(datatype, identifier);

  // consume comma that separates this parameter from the next one
  // the caller is responsible for calling this method again for parsing the next parameter
  if (it->hasValue(stork::reservedTokens::comma)) {
    parseTokenValue(it, stork::reservedTokens::comma);
  }

  return functionParameter;
}

Function *Parser::parseFunctionStatement(stork::tokens_iterator &it) {
  // consume 'public'
  parseTokenValue(it, stork::reservedTokens::kw_public);

  // parse return type
  auto datatype = parseDatatype(it);

  // parse function name
  auto functionName = parseIdentifier(it);

  // parse function parameters
  parseTokenValue(it, stork::reservedTokens::open_round);
  std::vector<std::unique_ptr<FunctionParameter>> functionParams;
  while (!it->hasValue(stork::reservedTokens::close_round)) {
    functionParams.push_back(std::unique_ptr<FunctionParameter>(parseFunctionParameter(it)));
  }
  parseTokenValue(it, stork::reservedTokens::close_round);

  // parse block/body statements
  auto block = std::unique_ptr<Block>(parseBlockStatement(it));

  return new Function(datatype, functionName, std::move(functionParams), std::move(block));
}

For *Parser::parseForStatement(stork::tokens_iterator &it) {
  // TODO: Implement me!
  ++it; // make MSVC stop complaining about unused function param
  throw std::runtime_error("NOT IMPLEMENTED");
}

If *Parser::parseIfStatement(stork::tokens_iterator &it) {
  // TODO: Implement me!
  ++it; // make MSVC stop complaining about unused function param
  throw std::runtime_error("NOT IMPLEMENTED");
}

Return *Parser::parseReturnStatement(stork::tokens_iterator &it) {
  parseTokenValue(it, stork::reservedTokens::kw_return);

  // Is it a return; i.e. no return value?
  if (it->hasValue(stork::reservedTokens::semicolon)) {
    return new Return();
  } else {
    AbstractExpression *p = parseExpression(it);
    return new Return(std::unique_ptr<AbstractExpression>(p));
  }
}

Block *Parser::parseBlockStatement(stork::tokens_iterator &it) {
  // parse block/body statements
  parseTokenValue(it, stork::reservedTokens::open_curly);
  std::vector<std::unique_ptr<AbstractStatement>> blockStatements;
  while (!it->hasValue(stork::reservedTokens::close_curly)) {
    blockStatements.push_back(std::unique_ptr<AbstractStatement>(parseStatement(it)));
  }
  parseTokenValue(it, stork::reservedTokens::close_curly);
  return new Block(std::move(blockStatements));
}

VariableDeclaration *Parser::parseVariableDeclarationStatement(stork::tokens_iterator &it) {
  // the variable's datatype
  auto datatype = parseDatatype(it);

  // the variable's name
  auto variable = std::unique_ptr<Variable>(parseVariable(it));

  // the variable's assigned value, if any assigned
  if (!it->hasValue(stork::reservedTokens::semicolon)) {
    parseTokenValue(it, stork::reservedTokens::assign);
    AbstractExpression *value = parseExpression(it);
    // the trailing semicolon
    parseTokenValue(it, stork::reservedTokens::semicolon);
    return new VariableDeclaration(datatype, std::move(variable), std::unique_ptr<AbstractExpression>(value));
  } else {
    // the trailing semicolon
    parseTokenValue(it, stork::reservedTokens::semicolon);
    return new VariableDeclaration(datatype, std::move(variable));
  }
}

Assignment *Parser::parseAssignmentStatement(stork::tokens_iterator &it) {
  // the target's name
  auto target = std::unique_ptr<AbstractTarget>(parseTarget(it));

  // the variable's assigned value
  parseTokenValue(it, stork::reservedTokens::assign);
  AbstractExpression *value = parseExpression(it);

  // the trailing semicolon
  parseTokenValue(it, stork::reservedTokens::semicolon);

  return new Assignment(std::move(target), std::unique_ptr<AbstractExpression>(value));
}
