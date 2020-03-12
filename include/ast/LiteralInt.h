#ifndef AST_OPTIMIZER_INCLUDE_AST_LITERALINT_H_
#define AST_OPTIMIZER_INCLUDE_AST_LITERALINT_H_

#include <string>
#include <ostream>
#include <map>
#include "AbstractLiteral.h"

template<typename T>
class Matrix;
class AbstractMatrix;

class LiteralInt : public AbstractLiteral {
 private:
  /// Stores the values of this LiteralInt. Is usually a Matrix<int> but can also be a Matrix<AbstractExpr*> in the case
  /// that this matrix contains unevaluated expression, e.g., GetMatrixElement.
  AbstractMatrix *matrix;

 public:
  explicit LiteralInt(Matrix<AbstractExpr *> *am);

  explicit LiteralInt(Matrix<int> *inputMatrix);

  explicit LiteralInt(int value);

  ~LiteralInt() override;

  LiteralInt *clone(bool keepOriginalUniqueNodeId) override;

  [[nodiscard]] int getValue() const;

  [[nodiscard]] json toJson() const override;

  void accept(Visitor &v) override;

  [[nodiscard]] std::string getNodeType() const override;

  bool operator==(const LiteralInt &rhs) const;

  bool operator!=(const LiteralInt &rhs) const;

  bool supportsDatatype(Datatype &datatype) override;

  void addLiteralValue(std::string identifier, std::unordered_map<std::string, AbstractLiteral *> &paramsMap) override;

  void setRandomValue(RandLiteralGen &rlg) override;

  void setValue(int newValue);

  [[nodiscard]] std::string toString(bool printChildren) const override;

  void print(std::ostream &str) const override;

  bool supportsCircuitMode() override;

  bool isEqual(AbstractExpr *other) override;

  bool isNull() override;

  [[nodiscard]] AbstractMatrix *getMatrix() const override;

  void setMatrix(AbstractMatrix *newValue) override;
};

#endif //AST_OPTIMIZER_INCLUDE_AST_LITERALINT_H_
