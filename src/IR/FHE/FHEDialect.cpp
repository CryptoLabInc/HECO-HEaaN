#include "mlir/IR/DialectImplementation.h"
#include "mlir/IR/Types.h"
#include "mlir/IR/TypeSupport.h"
#include "mlir/Dialect/EmitC/IR/EmitC.h"
#include "mlir/IR/PatternMatch.h"
#include "mlir/Support/LLVM.h"
#include "llvm/ADT/TypeSwitch.h"
#include "abc/IR/FHE/FHEDialect.h"

using namespace mlir;
using namespace fhe;

//===----------------------------------------------------------------------===//
// TableGen'd Type definitions
//===----------------------------------------------------------------------===//
#define GET_TYPEDEF_CLASSES
#include "abc/IR/FHE/FHETypes.cpp.inc"

SecretType BatchedSecretType::getCorrespondingSecretType() const {
  return SecretType::get(getContext(), getPlaintextType());
}

//===----------------------------------------------------------------------===//
// TableGen'd Operation definitions
//===----------------------------------------------------------------------===//

/// The 'OpAsmParser' class provides a collection of methods for parsing
/// various punctuation, as well as attributes, operands, types, etc. Each of
/// these methods returns a `ParseResult`. This class is a wrapper around
/// `LogicalResult` that can be converted to a boolean `true` value on failure,
/// or `false` on success. This allows for easily chaining together a set of
/// parser rules. These rules are used to populate an `mlir::OperationState`
/// similarly to the `build` methods described above.
static mlir::ParseResult parseCombineOp(mlir::OpAsmParser &parser,
                                        mlir::OperationState &result) {
  //mlir::DenseElementsAttr value;
  //if (parser.parseOptionalAttrDict(result.attributes) ||
  //    parser.parseAttribute(value, "value", result.attributes))
  //  return failure();
//
  //result.addTypes(value.getType());
  //return success();
  return failure(); //TODO: SUPPORT PARSING COMBINEDOP
}

/// The 'OpAsmPrinter' class is a stream that allows for formatting
/// strings, attributes, operands, types, etc.
static void print(mlir::OpAsmPrinter &printer, fhe::CombineOp op) {
  printer << "(";
  assert(op.vectors().size()==op.indices().size() && "combine op must have indices entry for each operand");
  auto indices = op.indices().getValue();
  for (size_t i = 0; i < op.vectors().size(); ++i) {
    if (i!=0)
      printer << ", ";
    printer.printOperand(op.getOperand(i));
    llvm::SmallVector<Attribute> attrs = {};
    if (auto aa = indices[i].dyn_cast_or_null<ArrayAttr>())
      for (auto a: aa)
        attrs.push_back(a);
    else
      attrs.push_back(indices[i].dyn_cast<IntegerAttr>());

    if (attrs.size()==1)
      if (auto ia = attrs.front().dyn_cast_or_null<IntegerAttr>())
        if (ia.getSInt() >= 0)
          printer << "#" << ia.getSInt();

    //TODO: Support printing out multiple indices
  }
  printer << ") : ";
  printer.printType(op.getType());
  printer << " // GENERIC:";
  printer.printGenericOp(op);
}

#define GET_OP_CLASSES
#include "abc/IR/FHE/FHE.cpp.inc"

::mlir::LogicalResult fhe::MultiplyOp::inferReturnTypes(::mlir::MLIRContext *context,
                                                        ::llvm::Optional<::mlir::Location> location,
                                                        ::mlir::ValueRange operands,
                                                        ::mlir::DictionaryAttr attributes,
                                                        ::mlir::RegionRange regions,
                                                        ::llvm::SmallVectorImpl<::mlir::Type> &inferredReturnTypes) {
  // Operand adaptors (https://mlir.llvm.org/docs/OpDefinitions/#operand-adaptors) provide a convenient way to access operands
  // when given as a "generic" triple of ValueRange, DictionaryAttr, RegionRange  instead of nicely "packaged" inside the operation class.
  auto op = MultiplyOpAdaptor(operands, attributes, regions);
  auto plaintextType = Type();
  bool batched = false;
  for (auto operand: op.x()) {
    if (auto secret_type = operand.getType().dyn_cast_or_null<SecretType>()) {
      plaintextType = secret_type.getPlaintextType();
    }
    if (auto bst = operand.getType().dyn_cast_or_null<BatchedSecretType>()) {
      plaintextType = bst.getPlaintextType();
      batched = true;
    }
    //TODO: check things properly!
  }
  if (batched)
    inferredReturnTypes.push_back(BatchedSecretType::get(context, plaintextType));
  else
    inferredReturnTypes.push_back(SecretType::get(context, plaintextType));
  return ::mlir::success();
}

::mlir::LogicalResult fhe::AddOp::inferReturnTypes(::mlir::MLIRContext *context,
                                                   ::llvm::Optional<::mlir::Location> location,
                                                   ::mlir::ValueRange operands,
                                                   ::mlir::DictionaryAttr attributes,
                                                   ::mlir::RegionRange regions,
                                                   ::llvm::SmallVectorImpl<::mlir::Type> &inferredReturnTypes) {
  // Operand adaptors (https://mlir.llvm.org/docs/OpDefinitions/#operand-adaptors) provide a convenient way to access operands
  // when given as a "generic" triple of ValueRange, DictionaryAttr, RegionRange  instead of nicely "packaged" inside the operation class.
  auto op = AddOpAdaptor(operands, attributes, regions);
  auto plaintextType = Type();
  bool batched = false;
  for (auto operand: op.x()) {
    if (auto secret_type = operand.getType().dyn_cast_or_null<SecretType>()) {
      plaintextType = secret_type.getPlaintextType();
    }
    if (auto bst = operand.getType().dyn_cast_or_null<BatchedSecretType>()) {
      plaintextType = bst.getPlaintextType();
      batched = true;
    }
    //TODO: check things properly!
  }
  if (batched)
    inferredReturnTypes.push_back(BatchedSecretType::get(context, plaintextType));
  else
    inferredReturnTypes.push_back(SecretType::get(context, plaintextType));
  return ::mlir::success();
}

::mlir::LogicalResult fhe::SubOp::inferReturnTypes(::mlir::MLIRContext *context,
                                                   ::llvm::Optional<::mlir::Location> location,
                                                   ::mlir::ValueRange operands,
                                                   ::mlir::DictionaryAttr attributes,
                                                   ::mlir::RegionRange regions,
                                                   ::llvm::SmallVectorImpl<::mlir::Type> &inferredReturnTypes) {
  // Operand adaptors (https://mlir.llvm.org/docs/OpDefinitions/#operand-adaptors) provide a convenient way to access operands
  // when given as a "generic" triple of ValueRange, DictionaryAttr, RegionRange  instead of nicely "packaged" inside the operation class.
  auto op = SubOpAdaptor(operands, attributes, regions);
  auto plaintextType = Type();
  bool batched = false;
  for (auto operand: op.x()) {
    if (auto secret_type = operand.getType().dyn_cast_or_null<SecretType>()) {
      plaintextType = secret_type.getPlaintextType();
    }
    if (auto bst = operand.getType().dyn_cast_or_null<BatchedSecretType>()) {
      plaintextType = bst.getPlaintextType();
      batched = true;
    }
    //TODO: check things properly!
  }
  if (batched)
    inferredReturnTypes.push_back(BatchedSecretType::get(context, plaintextType));
  else
    inferredReturnTypes.push_back(SecretType::get(context, plaintextType));
  return ::mlir::success();
}

::mlir::LogicalResult fhe::ConstOp::inferReturnTypes(::mlir::MLIRContext *context,
                                                     ::llvm::Optional<::mlir::Location> location,
                                                     ::mlir::ValueRange operands,
                                                     ::mlir::DictionaryAttr attributes,
                                                     ::mlir::RegionRange regions,
                                                     ::llvm::SmallVectorImpl<::mlir::Type> &inferredReturnTypes) {
  // Operand adaptors (https://mlir.llvm.org/docs/OpDefinitions/#operand-adaptors) provide a convenient way to access operands
  // when given as a "generic" triple of ValueRange, DictionaryAttr, RegionRange  instead of nicely "packaged" inside the operation class.
  auto op = ConstOpAdaptor(operands, attributes, regions);
  if (auto da = op.value().dyn_cast_or_null<DenseElementsAttr>()) {
    inferredReturnTypes.push_back(fhe::BatchedSecretType::get(context, da.getElementType()));
  } else {
    inferredReturnTypes.push_back(fhe::SecretType::get(context, op.value().getType()));
  }
  return ::mlir::success();
}

void fhe::ConstOp::getAsmResultNames(
    function_ref<void(Value, StringRef)> setNameFn) {
  auto type = Type();
  if (getType().isa<SecretType>())
    type = getType().cast<SecretType>().getPlaintextType();
  else
    type = getType().cast<BatchedSecretType>().getPlaintextType();

  if (auto intCst = value().dyn_cast<IntegerAttr>()) {
    auto intType = type.dyn_cast<IntegerType>();

    // Sugar i1 constants with 'true' and 'false'.
    if (intType && intType.getWidth()==1)
      return setNameFn(getResult(), (intCst.getInt() ? "true" : "false"));

    // Otherwise, build a complex name with the value and type.
    SmallString<32> specialNameBuffer;
    llvm::raw_svector_ostream specialName(specialNameBuffer);
    specialName << "c" << intCst.getInt();
    if (intType)
      specialName << '_' << type;
    setNameFn(getResult(), specialName.str());
  } else if (auto fCst = value().dyn_cast<FloatAttr>()) {
    auto floatType = type.dyn_cast<FloatType>();
    SmallString<32> specialNameBuffer;
    llvm::raw_svector_ostream specialName(specialNameBuffer);
    specialName << "c" << (int) fCst.getValueAsDouble();
    if (floatType)
      specialName << "_s" << type;
    setNameFn(getResult(), specialName.str());
  } else if (auto arrayCst = value().dyn_cast<ArrayAttr>()) {
    //TODO: Somehow support array stuff better?
    setNameFn(getResult(), "vcst");
  } else {
    setNameFn(getResult(), "cst");
  }
}

/// Simplifies
///  %os = materialize(%ctxt)->bst
///  %ex_op = extract(%os, i)
///  %op = materialize(%ex_op) -> ctxt
/// to
///  %op = rotate(%ctxt, -i)
::mlir::LogicalResult fhe::MaterializeOp::canonicalize(MaterializeOp op, ::mlir::PatternRewriter &rewriter) {

  if (auto ot = op.getType().dyn_cast_or_null<emitc::OpaqueType>()) {
    if (ot.getValue()=="seal::Ciphertext") {
      if (auto ex_op = op.input().getDefiningOp<fhe::ExtractOp>()) {
        if (auto original_source = ex_op.vector().getDefiningOp<MaterializeOp>()) {
          if (auto original_ot = original_source.input().getType().dyn_cast_or_null<emitc::OpaqueType>()) {
            if (original_ot.getValue()=="seal::Ciphertext") {
              // we now have something like this
              // %os = materialize(%ctxt)->bst
              // %ex_op = extract(%os, i)
              // %op = materialize(%ex_op) -> ctxt
              //
              // Instead of doing all of that, we can just change this to
              // %op = rotate(%ctxt, -i)
              //
              // This works because in ctxt land, there are no more scalars (result of extract)
              // and so "scalar" just means "what's in position 0 of the ctxt"
              //
              // Note that we don't actually remove the first materialize and ex_op,
              // since they'll be canonicalized away anyway as dead code if appropriate

              //rewriter.replaceOpWithNewOp<emitc::CallOp>(op, ex_op.vector(), -ex_op.i().getLimitedValue(INT32_MAX));
              auto i = (int) ex_op.i().getLimitedValue(INT32_MAX);
              auto a0 = rewriter.getIndexAttr(0); //stands for "first operand"
              auto a1 = rewriter.getSI32IntegerAttr(i);
              auto aa = ArrayAttr::get(rewriter.getContext(), {a0, a1});
              rewriter.replaceOpWithNewOp<emitc::CallOp>(op, TypeRange(ot),
                                                         llvm::StringRef("evaluator.rotate"),
                                                         aa,
                                                         ArrayAttr(),
                                                         ValueRange(original_source.input()));
              return success();
            }
          }
        }
      }
    }
  }
  return failure();
}

// replaces insert (extract %v1, i) into %v2, i  (note: i must match!) with combine (v1,v2) ([i], [-i])
// where [-i] means "everything except i"
::mlir::LogicalResult fhe::InsertOp::canonicalize(InsertOp op, ::mlir::PatternRewriter &rewriter) {
  if (auto ex_op = op.scalar().getDefiningOp<fhe::ExtractOp>()) {
    auto i = (int) ex_op.i().getLimitedValue(INT32_MAX);
    auto v1 = ex_op.vector();
    auto bst = ex_op.vector().getType().dyn_cast<fhe::BatchedSecretType>();
    assert(bst==op.dest().getType());
    if (i==(int) op.i().getLimitedValue(INT32_MAX)) {
      auto ai = rewriter.getSI32IntegerAttr(i);
      auto ami = rewriter.getStringAttr("all");
      auto aa = rewriter.getArrayAttr({ai, ami});
      rewriter.replaceOpWithNewOp<fhe::CombineOp>(op, bst, ValueRange({v1, op.dest()}), aa);
      return success();
    }
  }
  return failure();
}
// simplifies
//   %c = fhe.combine(%v#j, %w)
//   %op = fhe.combine(%v#i  %c)
// to
//   %op = fhe.combine(%v#[i,j], %w)
// Technically, the first combine op isn't removed, but if it has no other uses, it'll be canonicalized away, too
::mlir::OpFoldResult fhe::CombineOp::fold(::llvm::ArrayRef<::mlir::Attribute> operands) {
  bool updated = false;

  //getOperation()->getParentOp()->dump();
  //this->dump();

  assert(vectors().size()==indices().size() && "combine op must have indices foreach operand");

  if (vectors().size()==2)  // Currently, we only do this simple style. Will probably need to generalize later!
    if (auto c = vectors()[1].getDefiningOp<CombineOp>())
      if (c.vectors().size()==2)
        if (c.vectors()[0]==vectors()[0]) // matching vectors!
          if (auto sa = c.indices()[1].dyn_cast_or_null<StringAttr>())
            if (sa.getValue()=="all") { // other one determines the rest of the vector
              // We now have the desired conditions.
              int five = 4; // :-)
            }

//  auto vec = vectors()[i];
//  auto index = indices()[i];
//  // We only check
//  if (auto sa = index.dyn_cast_or_null<StringAttr>())
//    if (vi.getSInt() >= 0)
//      if (auto cop = vec.getDefiningOp<CombineOp>()) {
//        // For now, we handle only very simple cases: one 1-slot operand first, and another "all the rest" vector after.
//        // This happens to be what canonicalization of an insert produces, so this is still useful ;)
//        if (cop.vectors().size()==2) {
//          if (auto i0 = cop.indices()[0].dyn_cast_or_null<IntegerAttr>())
//            if (auto all = cop.indices()[1].dyn_cast_or_null<StringAttr>())
//              if (i0==vi && i0.getSInt() >= 0  /* <- always true because we checked vi >= 0 */
//                  && all.getValue()=="all") {
//                potential_vectors.push_back(cop.vectors()[0]);
//                potential_indices.push_back(i0);
//                potential_replaced.push_back(vec);
//              }
//        }
//      }
//
//}
//// Copy the current things
//SmallVector<Value> new_vectors = vectors();
//SmallVector<Attribute> new_indices;
//for (
//auto i
//:
//indices()
//)
//new_indices.
//push_back(i);
//// Replace all that were replaced
//for (
//size_t i = 0;
//i<potential_replaced.
//size();
//++i) {
//for (
//size_t j = 0;
//j<new_vectors.
//size();
//++j) {
//if (potential_replaced[i]==new_vectors[j]) {
//new_vectors[j] = potential_vectors[i];
//new_indices[j] = potential_indices[i];
//updated = true; //should be below, not here
//break; // no need to look for more matches for this replacee
//}
//}
//}
//
//// Now that we've updated everything, check if can combine anything!
//

// update the op
  if (updated) {
    return getResult();
  } else {
    return {};
  }
}

/// simplifies a constant operation to its value (used for constant folding?)
::mlir::OpFoldResult fhe::ConstOp::fold(::llvm::ArrayRef<::mlir::Attribute> operands) {
  return value();
}

/// simplifies away materialization(materialization(x)) to x if the types work
::mlir::OpFoldResult fhe::MaterializeOp::fold(::llvm::ArrayRef<::mlir::Attribute> operands) {
  if (auto m_op = input().getDefiningOp<fhe::MaterializeOp>())
    if (m_op.input().getType()==result().getType())
      return m_op.input();
  return {};
}

/// simplifies rotate(x,0) to x
::mlir::OpFoldResult fhe::RotateOp::fold(::llvm::ArrayRef<::mlir::Attribute> operands) {
  if (i()==0)
    return x();
  return {};
}
/// simplifies add(x,0) and add(x) to x
::mlir::OpFoldResult fhe::AddOp::fold(::llvm::ArrayRef<::mlir::Attribute> operands) {
  auto neutral_element = 0;
  SmallVector<Value> new_operands;
  for (auto v: x()) {
    bool omit = false;
    if (auto cst_op = v.getDefiningOp<fhe::ConstOp>()) {
      if (auto dea = cst_op.value().dyn_cast_or_null<DenseElementsAttr>()) {
        if (dea.size()==1) {
          if (dea.getElementType().isIntOrIndex()) {
            if (dea.value_begin<const IntegerAttr>()->getInt()==neutral_element)
              omit = true;
          } else if (dea.getElementType().isIntOrFloat()) {
            //because we've already excluded IntOrIndex, it must be float
            if (dea.value_begin<const FloatAttr>()->getValueAsDouble()==neutral_element)
              omit = true;
          }
        }
      } else if (auto ia = cst_op.value().dyn_cast_or_null<IntegerAttr>()) {
        if (ia.getInt()==neutral_element)
          omit = true;
      } else if (auto fa = cst_op.value().dyn_cast_or_null<FloatAttr>()) {
        if (fa.getValueAsDouble()==neutral_element)
          omit = true;
      }
    }
    if (!omit)
      new_operands.push_back(v);
  }
  xMutable().assign(new_operands);
  if (x().size() > 1)
    return getResult();
  else
    return x().front();
}
/// simplifies sub(x,0) and sub(x) to x
::mlir::OpFoldResult fhe::SubOp::fold(::llvm::ArrayRef<::mlir::Attribute> operands) {
  auto neutral_element = 0;
  SmallVector<Value> new_operands;
  for (auto v: x()) {
    bool omit = false;
    if (auto cst_op = v.getDefiningOp<fhe::ConstOp>()) {
      if (auto dea = cst_op.value().dyn_cast_or_null<DenseElementsAttr>()) {
        if (dea.size()==1) {
          if (dea.getElementType().isIntOrIndex()) {
            if (dea.value_begin<const IntegerAttr>()->getInt()==neutral_element)
              omit = true;
          } else if (dea.getElementType().isIntOrFloat()) {
            //because we've already excluded IntOrIndex, it must be float
            if (dea.value_begin<const FloatAttr>()->getValueAsDouble()==neutral_element)
              omit = true;
          }
        }
      } else if (auto ia = cst_op.value().dyn_cast_or_null<IntegerAttr>()) {
        if (ia.getInt()==neutral_element)
          omit = true;
      } else if (auto fa = cst_op.value().dyn_cast_or_null<FloatAttr>()) {
        if (fa.getValueAsDouble()==neutral_element)
          omit = true;
      }
    }
    if (!omit)
      new_operands.push_back(v);
  }
  xMutable().assign(new_operands);
  if (x().size() > 1)
    return getResult();
  else
    return x().front();
}
/// simplifies mul(x,1) and mul(x) to x
::mlir::OpFoldResult fhe::MultiplyOp::fold(::llvm::ArrayRef<::mlir::Attribute> operands) {
  auto neutral_element = 1;
  SmallVector<Value> new_operands;
  for (auto v: x()) {
    bool omit = false;
    if (auto cst_op = v.getDefiningOp<fhe::ConstOp>()) {
      if (auto dea = cst_op.value().dyn_cast_or_null<DenseElementsAttr>()) {
        if (dea.size()==1) {
          if (dea.getElementType().isIntOrIndex()) {
            if (dea.value_begin<const IntegerAttr>()->getInt()==neutral_element)
              omit = true;
          } else if (dea.getElementType().isIntOrFloat()) {
            //because we've already excluded IntOrIndex, it must be float
            if (dea.value_begin<const FloatAttr>()->getValueAsDouble()==neutral_element)
              omit = true;
          }
        }
      } else if (auto ia = cst_op.value().dyn_cast_or_null<IntegerAttr>()) {
        if (ia.getInt()==neutral_element)
          omit = true;
      } else if (auto fa = cst_op.value().dyn_cast_or_null<FloatAttr>()) {
        if (fa.getValueAsDouble()==neutral_element)
          omit = true;
      }
    }
    if (!omit)
      new_operands.push_back(v);
  }
  xMutable().assign(new_operands);
  if (x().size() > 1)
    return getResult();
  else
    return x().front();
}

//===----------------------------------------------------------------------===//
// FHE dialect definitions
//===----------------------------------------------------------------------===//
#include "abc/IR/FHE/FHEDialect.cpp.inc"
void FHEDialect::initialize() {

  // Registers all the Types into the FHEDialect class
  addTypes<
#define GET_TYPEDEF_LIST
#include "abc/IR/FHE/FHETypes.cpp.inc"
  >();

  // Registers all the Operations into the FHEDialect class
  addOperations<
#define GET_OP_LIST
#include "abc/IR/FHE/FHE.cpp.inc"
  >();

}

// TODO (Q&A): The <dialect>::parseType function seem to be generic boilerplate. Can we make TableGen generate them for us?
mlir::Type FHEDialect::parseType(::mlir::DialectAsmParser &parser) const {
  mlir::StringRef typeTag;
  if (parser.parseKeyword(&typeTag))
    return {};
  mlir::Type genType;
  auto parseResult = generatedTypeParser(parser, typeTag, genType);
  if (parseResult.hasValue())
    return genType;
  parser.emitError(parser.getNameLoc(), "unknown fhe type: ") << typeTag;
  return {};
}

// TODO (Q&A): The <dialect>::printType function seem to be generic boilerplate. Can we make TableGen generate them for us?
void FHEDialect::printType(::mlir::Type type, ::mlir::DialectAsmPrinter &os) const {
  if (mlir::failed(generatedTypePrinter(type, os)))
    llvm::report_fatal_error("unknown type to print");
}