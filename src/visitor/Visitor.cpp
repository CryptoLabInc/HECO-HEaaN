#include <ast_opt/visitor/Visitor.h>

void ScopedVisitor::visit(BinaryExpression &) {}

void ScopedVisitor::visit(Block &) {}

void ScopedVisitor::visit(For &) {}

void ScopedVisitor::visit(FunctionParameter &) {}

void ScopedVisitor::visit(If &) {}

void ScopedVisitor::visit(LiteralBool &) {}

void ScopedVisitor::visit(LiteralChar &) {}

void ScopedVisitor::visit(LiteralInt &) {}

void ScopedVisitor::visit(LiteralFloat &) {}

void ScopedVisitor::visit(LiteralDouble &) {}

void ScopedVisitor::visit(LiteralString &) {}

void ScopedVisitor::visit(UnaryExpression &) {}

void ScopedVisitor::visit(VariableAssignment &) {}

void ScopedVisitor::visit(VariableDeclaration &) {}

void ScopedVisitor::visit(Variable &) {}