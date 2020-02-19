#ifndef AST_OPTIMIZER_EVALUATIONPRINTER_H
#define AST_OPTIMIZER_EVALUATIONPRINTER_H

#include <unordered_map>
#include <string>
#include "AbstractLiteral.h"

class EvaluationPrinter {
 private:
  // flags to configure the amount of data to be printed
  bool flagPrintEachParameterSet{false};
  bool flagPrintVariableHeaderOnceOnly{true};
  bool flagPrintEvaluationResult{false};

  // the evaluation parameters associated to the test run
  std::unordered_map<std::string, AbstractLiteral *> *evaluationParameters;

  void ensureEvalParamsAreSet();

 public:
  EvaluationPrinter();

  EvaluationPrinter &setFlagPrintEachParameterSet(bool printEachParameterSet);

  EvaluationPrinter &setFlagPrintVariableHeaderOnceOnly(bool printVariableHeaderOnceOnly);

  EvaluationPrinter &setEvaluationParameters(std::unordered_map<std::string, AbstractLiteral *> *evalParams);

  EvaluationPrinter &setFlagPrintEvaluationResult(bool printEvaluationResult);

  void printHeader();

  void printCurrentParameterSet();

  void printEvaluationResults(const std::vector<AbstractLiteral *> &resultExpected,
                              const std::vector<AbstractLiteral *> &resultRewrittenAst);

  static void printEndOfEvaluationTestRun();
};

#endif //AST_OPTIMIZER_EVALUATIONPRINTER_H