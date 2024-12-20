

#include "ir.h"
#include "assembler.h"
#include "opcode.h"
#include "types.h"
#include "util.h"
#include <cassert>
#include <iomanip>
#include <linux/limits.h>
#include <ostream>

int IR::_ljust = 10;

#define VAR_NAME_W 3
#define LJUST(n) std::left << std::setw(n)

void BinaryIR::Emit(CVMAssembler &assembler) const
{
  _rhs->Emit(assembler);
  _lhs->Emit(assembler);
  assembler.WriteStream(static_cast<u2>(GetOpCode()));
}

void VariableIR::Emit(CVMAssembler &assembler) const
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

void Constant::Emit(CVMAssembler &assembler) const
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

void StoreIR::Emit(CVMAssembler &assembler) const
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

void ProgramIR::Emit(CVMAssembler &assembler) const
{
  for (const auto &ir : _irs)
  {
    ir->Emit(assembler);
  }
}

void AssignStmtIR::Emit(CVMAssembler &assembler) const
{
  // post order traversal
  _rhs->Emit(assembler);
  _lhs->Emit(assembler);
}

void DefinitionIR::Emit(CVMAssembler &assembler) const
{
}

void NopIR::Emit(CVMAssembler &assembler) const
{
}

void CastIR::Emit(CVMAssembler &assembler) const
{
  _ir->Emit(assembler);
  assembler.WriteStream(static_cast<u2>(GetOpCode()));
}

void FunctionCallIR::Emit(CVMAssembler &assembler) const
{
  if (_func_name != "print")
  {
    panic("only print can be used in cvm", __FILE__, __LINE__);
  }
  if (_args.size() != 1)
  {
    panic("print only support one argument", __FILE__, __LINE__);
  }
  _args[0]->Emit(assembler);
  const auto &arg0 = _args[0];
  if (arg0->IsDoubleRes())
  {
    assembler.WriteStream(static_cast<u2>(OpCode::kPrintD));
  }
  else if (arg0->IsFloatRes())
  {
    assembler.WriteStream(static_cast<u2>(OpCode::kPrintF));
  }
  else if (arg0->IsIntRes())
  {
    assembler.WriteStream(static_cast<u2>(OpCode::kPrintI));
  }
  else if (arg0->IsLongRes())
  {
    assembler.WriteStream(static_cast<u2>(OpCode::kPrintL));
  }
  assembler.WriteStream(static_cast<u2>(_args.size()));
}

void BinaryIR::Dump(std::ostream &os) const
{
  _lhs->Dump(os);
  os << "\n";
  _rhs->Dump(os);
  os << "\n";
  switch (GetOpCode())
  {
  case OpCode::kIAdd:
    os << "IAdd";
    break;
  case OpCode::kISub:
    os << "ISub";
    break;
  case OpCode::kIMul:
    os << "IMul";
    break;
  case OpCode::kIDiv:
    os << "IDiv";
    break;
  case OpCode::kIMod:
    os << "IMod";
    break;
  case OpCode::kDAdd:
    os << "DAdd";
    break;
  case OpCode::kDSub:
    os << "DSub";
    break;
  case OpCode::kDMul:
    os << "DMul";
    break;
  case OpCode::kDDiv:
    os << "DDiv";
    break;
  case OpCode::kFAdd:
    os << "FAdd";
    break;
  case OpCode::kLAdd:
    os << "LAdd";
    break;
  case OpCode::kLSub:
    os << "LSub";
    break;
  case OpCode::kLMul:
    os << "LMul";
    break;
  case OpCode::kLDiv:
    os << "LDiv";
    break;
  case OpCode::kLMod:
    os << "LMod";
    break;
  default:
    panic("unknown or unimplemented binary op", __FILE__, __LINE__);
  }
}

void VariableIR::Dump(std::ostream &os) const
{
  switch (GetOpCode())
  {
  case OpCode::kLoadI:
    os << LJUST(IR::_ljust) << "LoadI" << LJUST(VAR_NAME_W) << GetIx() << "("
       << GetName() << ")";
    break;
  case OpCode::kLoadF:
    os << LJUST(IR::_ljust) << "LoadF" << LJUST(VAR_NAME_W) << GetIx() << "("
       << GetName() << ")";
    break;
  case OpCode::kLoadD:
    os << LJUST(IR::_ljust) << "LoadD" << LJUST(VAR_NAME_W) << GetIx() << "("
       << GetName() << ")";
    break;
  case OpCode::kLoadL:
    os << LJUST(IR::_ljust) << "LoadL" << LJUST(VAR_NAME_W) << GetIx() << "("
       << GetName() << ")";
    break;
  default:
    panic("unknown or unimplemented load op", __FILE__, __LINE__);
  }
}

void Constant::Dump(std::ostream &os) const
{
  switch (GetOpCode())
  {
  case OpCode::kIConst:
    os << LJUST(IR::_ljust) << "IConst" << _val.GetIntADT();
    break;
  case OpCode::kFConst:
    os << LJUST(IR::_ljust) << "FConst" << _val.GetFloatADT();
    break;
  case OpCode::kDConst:
    os << LJUST(IR::_ljust) << "DConst" << _val.GetDoubleADT();
    break;
  case OpCode::kLConst:
    os << LJUST(IR::_ljust) << "LConst" << _val.GetLongADT();
    break;
  default:
    panic("unknown or unimplemented type", __FILE__, __LINE__);
  }
}

void StoreIR::Dump(std::ostream &os) const
{
  switch (GetOpCode())
  {
  case OpCode::kStoreI:
    os << LJUST(IR::_ljust) << "StoreI" << LJUST(VAR_NAME_W) << GetIx() << "("
       << GetName() << ")";
    break;
  case OpCode::kStoreF:
    os << LJUST(IR::_ljust) << "StoreF" << LJUST(VAR_NAME_W) << GetIx() << "("
       << GetName() << ")";
    break;
  case OpCode::kStoreD:
    os << LJUST(IR::_ljust) << "StoreD" << LJUST(VAR_NAME_W) << GetIx() << "("
       << GetName() << ")";
    break;
  case OpCode::kStoreL:
    os << LJUST(IR::_ljust) << "StoreL" << LJUST(VAR_NAME_W) << GetIx() << "("
       << GetName() << ")";
    break;
  default:
    panic("unknown or unimplemented store op", __FILE__, __LINE__);
  }
}

void ProgramIR::Dump(std::ostream &os) const
{
  os << LJUST(IR::_ljust) << "OpName" << LJUST(VAR_NAME_W)
     << "OpArg(VarName)\n";
  for (const auto &ir : _irs)
  {
    ir->Dump(os);
    os << "\n";
  }
}

void AssignStmtIR::Dump(std::ostream &os) const
{
  _rhs->Dump(os);
  os << "\n";
  _lhs->Dump(os);
}

void DefinitionIR::Dump(std::ostream &os) const
{
}

void NopIR::Dump(std::ostream &os) const
{
}

void CastIR::Dump(std::ostream &os) const
{
  _ir->Dump(os);
  os << "\n";
  switch (GetOpCode())
  {
  case OpCode::kICastD:
    os << "ICastD";
    break;
  case OpCode::kICastF:
    os << "ICastF";
    break;
  case OpCode::kICastL:
    os << "ICastL";
    break;
  case OpCode::kLCastD:
    os << "LCastD";
    break;
  case OpCode::kLCastF:
    os << "LCastF";
    break;
  case OpCode::kLCastI:
    os << "LCastI";
    break;
  case OpCode::kFCastD:
    os << "FCastD";
    break;
  case OpCode::kFCastI:
    os << "FCastI";
    break;
  case OpCode::kFCastL:
    os << "FCastL";
    break;
  case OpCode::kDCastI:
    os << "DCastI";
    break;
  case OpCode::kDCastL:
    os << "DCastL";
    break;
  case OpCode::kDCastF:
    os << "DCastF";
    break;
  default:
    panic("unknown or unimplemented cast op", __FILE__, __LINE__);
  }
}

void FunctionCallIR::Dump(std::ostream &os) const
{
  const auto &arg0 = _args[0];
  arg0->Dump(os);
  os << "\n";
  assert(_func_name == "print");
  if (arg0->IsDoubleRes())
  {
    os << "PrintD";
  }
  else if (arg0->IsFloatRes())
  {
    os << "PrintF";
  }
  else if (arg0->IsIntRes())
  {
    os << "PrintI";
  }
  else if (arg0->IsLongRes())
  {
    os << "PrintL";
  }
  else
  {
    panic("unknown or unimplemented print type", __FILE__, __LINE__);
  }
}

void BinaryIR::Verify() const
{
  _lhs->Verify();
  _rhs->Verify();
  if (_lhs->GetResType() != _rhs->GetResType())
    panic("binary op type mismatch", __FILE__, __LINE__);
  switch (GetOpCode())
  {
  case OpCode::kIAdd:
  case OpCode::kISub:
  case OpCode::kIMul:
  case OpCode::kIDiv:
  case OpCode::kIMod:
    if (_lhs->GetResType() != CVMType::GetOrCreateType("int"))
      panic("binary op type mismatch(int is required)", __FILE__, __LINE__);
    break;
  case OpCode::kDAdd:
  case OpCode::kDSub:
  case OpCode::kDMul:
  case OpCode::kDDiv:
    if (_lhs->GetResType() != CVMType::GetOrCreateType("double"))
      panic("binary op type mismatch(double is required)", __FILE__, __LINE__);
    break;
  case OpCode::kFAdd:
  case OpCode::kFSub:
  case OpCode::kFMul:
  case OpCode::kFDiv:
    if (_lhs->GetResType() != CVMType::GetOrCreateType("float"))
      panic("binary op type mismatch(float is required)", __FILE__, __LINE__);
  case OpCode::kLAdd:
  case OpCode::kLSub:
  case OpCode::kLMul:
  case OpCode::kLDiv:
  case OpCode::kLMod:
    if (_lhs->GetResType() != CVMType::GetOrCreateType("long"))
      panic("binary op type mismatch(long is required)", __FILE__, __LINE__);
  default:
    panic("unknown or unimplemented binary op", __FILE__, __LINE__);
  }
}

void VariableIR::Verify() const
{
}

void Constant::Verify() const
{
}

void StoreIR::Verify() const
{
}

void ProgramIR::Verify() const
{
  for (const auto &ir : _irs)
  {
    ir->Verify();
  }
}

void AssignStmtIR::Verify() const
{
  _lhs->Verify();
  _rhs->Verify();
  if (_lhs->GetResType() != _rhs->GetResType())
    panic("assign type mismatch", __FILE__, __LINE__);
}

void DefinitionIR::Verify() const
{
}

void NopIR::Verify() const
{
}

void CastIR::Verify() const
{
  _ir->Verify();
}

void FunctionCallIR::Verify() const
{
  if (_args.size() != 1)
    panic("function call args size error", __FILE__, __LINE__);
  _args[0]->Verify();
}