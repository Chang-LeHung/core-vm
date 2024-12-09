
#pragma once

enum class OpCode
{
  kIAdd,
  kISub,
  kIMul,
  kIDiv,
  kIMod,
  kDAdd,
  kDSub,
  kDMul,
  kDDiv,
  kDMod,
  kFAdd,
  kFSub,
  kFMul,
  kFDiv,
  kIConst,
  kLConst,
  kFConst,
  kDConst,
  kLoadI,
  kLoadD,
  kLoadF,
  kLoadL,
  kStoreI,
  kStoreD,
  kStoreF,
  kStoreL,
  kNop,
  kProgram, // not bytecode
  kStmt,    // not bytecode
  kDef,     // not bytecode
};
