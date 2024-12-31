
#include "bytecodeasm.h"
#include "opcode.h"
#include "types.h"
#include <cstring>

AsmInterpreter::AsmInterpreter(const char *code, int code_size, int stack_size,
                               int local_var_size)
{
  _stack_size = stack_size;
  _local_vars_size = local_var_size;

  _stack = (addr) new addr[_stack_size];
  _local_vars = (addr) new addr[_local_vars_size];
  _code = (u1 *)new u1[code_size];
  std::memcpy(_code, code, code_size);

  _bc_entry[static_cast<u4>(OpCode::kDone)] = done;
  _bc_entry[static_cast<u4>(OpCode::kIAdd)] = iadd;
  _bc_entry[static_cast<u4>(OpCode::kISub)] = isub;
  _bc_entry[static_cast<u4>(OpCode::kIMul)] = imul;
  _bc_entry[static_cast<u4>(OpCode::kIDiv)] = idiv;
  _bc_entry[static_cast<u4>(OpCode::kStoreI)] = storei;
  _bc_entry[static_cast<u4>(OpCode::kPrintI)] = printi;
  _bc_entry[static_cast<u4>(OpCode::kIConst)] = iconst;
  _bc_entry[static_cast<u4>(OpCode::kLoadI)] = loadi;
}

void AsmInterpreter::Run()
{
  trampoline((u1 *)_code, (u1 *)_bc_entry, (u1 *)_local_vars, (u1 *)_stack);
}

AsmInterpreter::~AsmInterpreter()
{
  delete[] (u8 *)_stack;
  delete[] (u8 *)_local_vars;
  delete[] _code;
}