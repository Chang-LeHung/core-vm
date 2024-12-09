

#include "ir.h"
#include "assembler.h"
#include "types.h"
#include "util.h"
#include <cassert>

void BinaryIR::emit(CVMAssembler &assembler) const
{
  _rhs->emit(assembler);
  _lhs->emit(assembler);
  switch (_op)
  {
  case OpCode::kIAdd:
    assembler.WriteStream(static_cast<u2>(OpCode::kIAdd));
    break;
  case OpCode::kISub:
    assembler.WriteStream(static_cast<u2>(OpCode::kISub));
    break;
  case OpCode::kIMul:
    assembler.WriteStream(static_cast<u2>(OpCode::kIMul));
    break;
  case OpCode::kIDiv:
    assembler.WriteStream(static_cast<u2>(OpCode::kIDiv));
    break;
  case OpCode::kIMod:
    assembler.WriteStream(static_cast<u2>(OpCode::kIMod));
    break;
  case OpCode::kDAdd:
    assembler.WriteStream(static_cast<u2>(OpCode::kDAdd));
    break;
  case OpCode::kDSub:
    assembler.WriteStream(static_cast<u2>(OpCode::kDSub));
    break;
  case OpCode::kDMul:
    assembler.WriteStream(static_cast<u2>(OpCode::kDMul));
    break;
  case OpCode::kDDiv:
    assembler.WriteStream(static_cast<u2>(OpCode::kDDiv));
    break;
  case OpCode::kDMod:
    assembler.WriteStream(static_cast<u2>(OpCode::kDMod));
    break;
  case OpCode::kFAdd:
    assembler.WriteStream(static_cast<u2>(OpCode::kFAdd));
    break;
  case OpCode::kFSub:
    assembler.WriteStream(static_cast<u2>(OpCode::kFSub));
    break;
  case OpCode::kFMul:
    assembler.WriteStream(static_cast<u2>(OpCode::kFMul));
    break;
  case OpCode::kFDiv:
    assembler.WriteStream(static_cast<u2>(OpCode::kFDiv));
    break;
  default:
    panic("unknown or unimplemented binary op", __FILE__, __LINE__);
  }
}

void VariableIR::emit(CVMAssembler &assembler) const
{
  if (IsIntRes())
    assembler.WriteStream(static_cast<u2>(OpCode::kLoadI));
  else if (IsFloatRes())
    assembler.WriteStream(static_cast<u2>(OpCode::kLoadF));
  else if (IsDoubleRes())
    assembler.WriteStream(static_cast<u2>(OpCode::kLoadD));
  else if (IsLongRes())
    assembler.WriteStream(static_cast<u2>(OpCode::kLoadL));
  else
    panic("unknown or unimplemented type", __FILE__, __LINE__);
  assembler.WriteStream(static_cast<u2>(_ix));
}

void Constant::emit(CVMAssembler &assembler) const
{
  switch (_op)
  {
  case OpCode::kIConst:
    assembler.WriteStream(static_cast<u2>(OpCode::kIConst));
    assembler.WriteStream(static_cast<u4>(_val.GetIntADT()));
    break;
  case OpCode::kFConst:
    assembler.WriteStream(static_cast<u2>(OpCode::kFConst));
    assembler.WriteStream(static_cast<f4>(_val.GetFloatADT()));
    break;
  case OpCode::kDConst:
    assembler.WriteStream(static_cast<u2>(OpCode::kDConst));
    assembler.WriteStream(static_cast<f8>(_val.GetDoubleADT()));
    break;
  case OpCode::kLConst:
    assembler.WriteStream(static_cast<u2>(OpCode::kLConst));
    assembler.WriteStream(static_cast<u8>(_val.GetLongADT()));
    break;
  default:
    panic("unknown or unimplemented type", __FILE__, __LINE__);
  }
}

void StoreIR::emit(CVMAssembler &assembler) const
{
  if (IsIntRes())
    assembler.WriteStream(static_cast<u2>(OpCode::kStoreI));
  else if (IsFloatRes())
    assembler.WriteStream(static_cast<u2>(OpCode::kStoreF));
  else if (IsDoubleRes())
    assembler.WriteStream(static_cast<u2>(OpCode::kStoreD));
  else if (IsLongRes())
    assembler.WriteStream(static_cast<u2>(OpCode::kStoreL));
  else
    panic("unknown or unimplemented type", __FILE__, __LINE__);
  assembler.WriteStream(static_cast<u2>(GetIx()));
}

void ProgramIR::emit(CVMAssembler &assembler) const
{
  for (const auto &ir : _irs)
  {
    ir->emit(assembler);
  }
}

void AssignStmtIR::emit(CVMAssembler &assembler) const
{
  // post order traversal
  _rhs->emit(assembler);
  _lhs->emit(assembler);
}

void DefinitionIR::emit(CVMAssembler &assembler) const
{
}

void NopIR::emit(CVMAssembler &assembler) const
{
}