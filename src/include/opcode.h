
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
  kFAdd,
  kFSub,
  kFMul,
  kFDiv,
  kLAdd,
  kLSub,
  kLMul,
  kLDiv,
  kLMod,
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
  kICastL,
  kICastD,
  kICastF,
  kLCastI,
  kLCastD,
  kLCastF,
  kFCastI,
  kFCastD,
  kFCastL,
  kDCastI,
  kDCastF,
  kDCastL,
  kPrintI,
  kPrintL,
  kPrintD,
  kPrintF,
  kNop,
  kProgram, // not bytecode
  kStmt,    // not bytecode
  kDef,     // not bytecode
};
