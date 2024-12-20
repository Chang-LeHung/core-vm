
#pragma once

enum class OpCode
{
  kIAdd,    // 0
  kISub,    // 1
  kIMul,    // 2
  kIDiv,    // 3
  kIMod,    // 4
  kDAdd,    // 5
  kDSub,    // 6
  kDMul,    // 7
  kDDiv,    // 8
  kFAdd,    // 9
  kFSub,    // 10
  kFMul,    // 11
  kFDiv,    // 12
  kLAdd,    // 13
  kLSub,    // 14
  kLMul,    // 15
  kLDiv,    // 16
  kLMod,    // 17
  kIConst,  // 18
  kLConst,  // 19
  kFConst,  // 20
  kDConst,  // 21
  kLoadI,   // 22
  kLoadD,   // 23
  kLoadF,   // 24
  kLoadL,   // 25
  kStoreI,  // 26
  kStoreD,  // 27
  kStoreF,  // 28
  kStoreL,  // 29
  kICastL,  // 30
  kICastD,  // 31
  kICastF,  // 32
  kLCastI,  // 33
  kLCastD,  // 34
  kLCastF,  // 35
  kFCastI,  // 36
  kFCastD,  // 37
  kFCastL,  // 38
  kDCastI,  // 39
  kDCastF,  // 40
  kDCastL,  // 41
  kPrintI,  // 42
  kPrintL,  // 43
  kPrintD,  // 44
  kPrintF,  // 45
  kNop,     // 46
  kProgram, // not bytecode
  kStmt,    // not bytecode
  kDef,     // not bytecode
};
